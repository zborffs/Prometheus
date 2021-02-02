"""
TestStatus Enumerated type
"""
@enum TestStatus begin
	Passed
	Failed
	NoStatus
end

"""
PositionData Struct
"""
mutable struct PositionData
	ordering::Vector{Float64}
	depth::Vector{Int64}
	rawNodes::Vector{Int64}
	leafNodes::Vector{Int64}
	nps::Vector{Float64}
	posType::Vector{Int64}
	gameStage::Vector{Int64}
	time::Vector{Int64}
	testStatus::TestStatus
	PositionData() = new(Float64[], Int64[], Int64[], Int64[], Float64[], Int64[], Int64[], Int64[], NoStatus)
	PositionData(ordering, depth, rawNodes, leafNodes, nps, posType, gameStage, time, testStatus) = new(ordering, depth, rawNodes, leafNodes, nps, posType, gameStage, time, testStatus)
end

"""
EngineData Struct
"""
mutable struct TestSuiteData
	name::String
	positionData::Vector{PositionData}
	TestSuiteData() = new("", PositionData[])
	TestSuiteData(name::String) = new(name, PositionData[])
	TestSuiteData(name::String, positionData::Array{PositionData,1}) = new(name, positionData)
end

function positionTypeToInt(posType)
	if posType == "closed"
		return 1
	elseif posType == "semi_closed"
		return 2
	elseif posType == "semi_open"
		return 3
	else # open
		return 4
	end
end

function gameStageToInt(gameStage)
	if gameStage == "opening"
		return 1
	elseif gameStage == "middle"
		return 2
	elseif gameStage == "early_end"
		return 3
	else # late_middle
		return 4
	end
end

function test_suite_data(testSuiteName, filename)
	# load all lines of the file 'lines'
	lines = readlines(filename)

	# create nominal return value
	ret = TestSuiteData(testSuiteName)
	pData = PositionData()

	for l in lines
		# If we get "passed" / "failed", the position is over
		if l == "passed"
			pData.testStatus = Passed
			push!(ret.positionData, pData)
			pData = PositionData()
			continue
		elseif l == "failed"
			pData.testStatus = Failed
			push!(ret.positionData, pData)
			pData = PositionData()
			continue
		end

		# Otherwise, this is another position, so extract the data
		line = split(l)
		orderingIndex = findfirst(x->x == "ordering", line) + 1
		depthIndex = findfirst(x->x == "depth", line) + 1
		rawNodesIndex = findfirst(x->x == "raw_nodes", line) + 1
		leafNodesIndex = findfirst(x->x == "leaf_nodes", line) + 1
		npsIndex = findfirst(x->x == "nps", line) + 1
		posTypeIndex = findfirst(x->x == "position_type", line) + 1
		gameStageIndex = findfirst(x->x == "game_stage", line) + 1
		timeIndex = findfirst(x->x == "time", line) + 1
		if line[orderingIndex] != "nan"
			if line[orderingIndex] == "0"
				push!(pData.ordering, 0.0)
			else
				push!(pData.ordering, parse(Float64, line[orderingIndex]))
			end
			push!(pData.depth, parse(Int64, line[depthIndex]))
			push!(pData.rawNodes, parse(Int64, line[rawNodesIndex]))
			push!(pData.leafNodes, parse(Int64, line[leafNodesIndex]))
			push!(pData.nps, parse(Float64, line[npsIndex]))
			push!(pData.posType, positionTypeToInt(line[posTypeIndex]))
			push!(pData.gameStage, positionTypeToInt(line[gameStageIndex]))
			push!(pData.time, parse(Int64, line[timeIndex]))
		end
	end

	return ret
end
