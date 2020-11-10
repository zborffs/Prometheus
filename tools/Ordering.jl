# This script reads the raw engine output from some test suite, performs
# statistical analysis on the output, and returns some plots
# More specifically, this script


# Import Packages
using Plots, Statistics, Distributions, StatsPlots, HDF5, ColorSchemes
plotly()

# Read raw data from file
lines = readlines("data/ordering_output.txt")

# Create a vector for each number of interest and fill with parsed data
ordering = Vector{Float64}(undef, 0)
depth = Vector{Int64}(undef, 0)
rawNodes = Vector{Int64}(undef, 0)
leafNodes = Vector{Int64}(undef, 0)
nps = Vector{Float64}(undef, 0)
posType = Vector{Int64}(undef, 0)
gameStage = Vector{Int64}(undef, 0)
time = Vector{Int64}(undef, 0)

for l in lines
	line = split(l)
	orderingIndex = findfirst(x-> x == "ordering", line) + 1
	depthIndex = findfirst(x-> x == "depth", line) + 1
	rawNodesIndex = findfirst(x->x == "raw_nodes", line) + 1
	leafNodesIndex = findfirst(x->x == "leaf_nodes", line) + 1
	npsIndex = findfirst(x->x == "nps", line) + 1
	posTypeIndex = findfirst(x-> x == "position_type", line) + 1
	gameStageIndex = findfirst(x-> x == "game_stage", line) + 1
	timeIndex = findfirst(x->x=="time", line) + 1
	if line[orderingIndex] != "nan"
		if line[orderingIndex] == "0"
			push!(ordering, 0.0)
		else
			try
				push!(ordering, parse(Float64, line[orderingIndex]))
			catch
				println("=-=-=-=-=-=-=-=-=-=-=")
				println(line)
				println(line[orderingIndex])
				throw()
			end
		end
		push!(depth, parse(Int64, line[depthIndex]))
		push!(rawNodes, parse(Int64, line[rawNodesIndex]))
		push!(leafNodes, parse(Int64, line[leafNodesIndex]))
		push!(nps, parse(Float64, line[npsIndex]))
		if line[posTypeIndex] == "closed"
			push!(posType, 1)
		elseif line[posTypeIndex] == "semi_closed"
			push!(posType, 2)
		elseif line[posTypeIndex] == "semi_open"
			push!(posType, 3)
		else # open
			push!(posType, 4)
		end
		if line[gameStageIndex] == "opening"
			push!(gameStage, 1)
		elseif line[gameStageIndex] == "middle"
			push!(gameStage, 2)
		elseif line[gameStageIndex] == "early_end"
			push!(gameStage, 3)
		else # late_middle
			push!(gameStage, 4)
		end
		push!(time, parse(Int64, line[timeIndex]))
	end
end

println("N (Number of Samples) = ", length(ordering))
println("-------- Game Stage --------")
println("- From Openings       = ", count(x->x==1, gameStage))
println("- From Mid-Game       = ", count(x->x==2, gameStage))
println("- From Early End      = ", count(x->x==3, gameStage))
println("- From Late End       = ", count(x->x==4, gameStage))
println("------- Position Type ------")
println("- From Closed         = ", count(x->x==1, posType))
println("- From Semi-Closed    = ", count(x->x==2, posType))
println("- From Semi-Open      = ", count(x->x==3, posType))
println("- From Open           = ", count(x->x==4, posType))
println()
println("Overall Analysis: ")
println("--------- Ordering --------")
println("- mean   = ", mean(ordering))
println("- median = ", median(ordering))
println("- std    = ", std(ordering))
println("---- Ordering/Raw Nodes ---")
println("- cor    = ", cor(ordering, rawNodes))
println("---- Ordering/Leaf Nodes --")
println("- cor    = ", cor(ordering, leafNodes))
println("------ Ordering/Depth -----")
println("- cor    = ", cor(ordering, depth))
println("------ Ordering/Time ------")
println("- cor    = ", cor(ordering, time))
histogram(ordering, title="Overall Ordering % Histogram", xlabel="Ordering %",
	ylabel="Frequency (# Occurrences)",
	palette=cgrad(ColorSchemes.berlin.colors))
openingIndices = findall(x->x == 1, gameStage)
if length(openingIndices) != 0
	println()
	println("Opening: ")
	println("N = ", length(openingIndices))
	gameStageOrdering = ordering[openingIndices]
	gameStageRawNodes = rawNodes[openingIndices]
	gameStageLeafNodes = leafNodes[openingIndices]
	gameStageDepth = depth[openingIndices]
	gameStageTime = time[openingIndices]
	println("--------- Ordering --------")
	println("- mean   = ", mean(gameStageOrdering))
	println("- median = ", median(gameStageOrdering))
	println("- std    = ", std(gameStageOrdering))
	println("---- Ordering/Raw Nodes ---")
	println("- cor    = ", cor(gameStageOrdering, gameStageRawNodes))
	println("---- Ordering/Leaf Nodes --")
	println("- cor    = ", cor(gameStageOrdering, gameStageLeafNodes))
	println("------ Ordering/Depth -----")
	println("- cor    = ", cor(gameStageOrdering, gameStageDepth))
	println("------ Ordering/Time ------")
	println("- cor    = ", cor(gameStageOrdering, gameStageTime))
	histogram(gameStageOrdering,
		title="Opening Move Ordering Histogram for \"BasicTestsSmall\" Dataset",
		xlabel="Ordering%", ylabel="Frequency (# Occurrence)",
		legend=:none, palette=cgrad(ColorSchemes.berlin.colors))
end
midgameIndices = findall(x->x == 2, gameStage)
if length(midgameIndices) != 0
	println()
	println("Midgame: ")
	println("N = ", length(midgameIndices))
	gameStageOrdering = ordering[midgameIndices]
	gameStageRawNodes = rawNodes[midgameIndices]
	gameStageLeafNodes = leafNodes[midgameIndices]
	gameStageDepth = depth[midgameIndices]
	gameStageTime = time[midgameIndices]
	println("--------- Ordering --------")
	println("- mean   = ", mean(gameStageOrdering))
	println("- median = ", median(gameStageOrdering))
	println("- std    = ", std(gameStageOrdering))
	println("---- Ordering/Raw Nodes ---")
	println("- cor    = ", cor(gameStageOrdering, gameStageRawNodes))
	println("---- Ordering/Leaf Nodes --")
	println("- cor    = ", cor(gameStageOrdering, gameStageLeafNodes))
	println("------ Ordering/Depth -----")
	println("- cor    = ", cor(gameStageOrdering, gameStageDepth))
	println("------ Ordering/Time ------")
	println("- cor    = ", cor(gameStageOrdering, gameStageTime))
	histogram(gameStageOrdering,
		title="Mid-Game Move Ordering Histogram for \"BasicTestsSmall\" Dataset",
		xlabel="Ordering%", ylabel="Frequency (# Occurrence)",
		legend=:none, palette=cgrad(ColorSchemes.berlin.colors))
end
earlyEndIndices = findall(x->x == 3, gameStage)
if length(earlyEndIndices) != 0
	println()
	println("Early End Game: ")
	println("N = ", length(earlyEndIndices))
	gameStageOrdering = ordering[earlyEndIndices]
	gameStageRawNodes = rawNodes[earlyEndIndices]
	gameStageLeafNodes = leafNodes[earlyEndIndices]
	gameStageDepth = depth[earlyEndIndices]
	gameStageTime = time[earlyEndIndices]
	println("--------- Ordering --------")
	println("- mean   = ", mean(gameStageOrdering))
	println("- median = ", median(gameStageOrdering))
	println("- std    = ", std(gameStageOrdering))
	println("---- Ordering/Raw Nodes ---")
	println("- cor    = ", cor(gameStageOrdering, gameStageRawNodes))
	println("---- Ordering/Leaf Nodes --")
	println("- cor    = ", cor(gameStageOrdering, gameStageLeafNodes))
	println("------ Ordering/Depth -----")
	println("- cor    = ", cor(gameStageOrdering, gameStageDepth))
	println("------ Ordering/Time ------")
	println("- cor    = ", cor(gameStageOrdering, gameStageTime))
	histogram(gameStageOrdering,
		title="Early End-Game Move Ordering Histogram for \"BasicTestsSmall\" Dataset",
		xlabel="Ordering%", ylabel="Frequency (# Occurrence)",
		legend=:none, palette=cgrad(ColorSchemes.berlin.colors))
end
lateEndIndices = findall(x->x == 4, gameStage)
if length(lateEndIndices) != 0
	println()
	println("Late End Game: ")
	println("N = ", length(lateEndIndices))
	gameStageOrdering = ordering[lateEndIndices]
	gameStageRawNodes = rawNodes[lateEndIndices]
	gameStageLeafNodes = leafNodes[lateEndIndices]
	gameStageDepth = depth[lateEndIndices]
	gameStageTime = time[lateEndIndices]
	println("--------- Ordering --------")
	println("- mean   = ", mean(gameStageOrdering))
	println("- median = ", median(gameStageOrdering))
	println("- std    = ", std(gameStageOrdering))
	println("---- Ordering/Raw Nodes ---")
	println("- cor    = ", cor(gameStageOrdering, gameStageRawNodes))
	println("---- Ordering/Leaf Nodes --")
	println("- cor    = ", cor(gameStageOrdering, gameStageLeafNodes))
	println("------ Ordering/Depth -----")
	println("- cor    = ", cor(gameStageOrdering, gameStageDepth))
	println("------ Ordering/Time ------")
	println("- cor    = ", cor(gameStageOrdering, gameStageTime))
	histogram(gameStageOrdering, title="Late End-Game Move Ordering Histogram for \"BasicTestsSmall\" Dataset",
		xlabel="Ordering %", ylabel="Frequency (# Occurrence)",
		legend=:none, palette=cgrad(ColorSchemes.berlin.colors))
end

# Save all vectors to hd5 file
hdf5FileName = "data/ParsedOrdering.hdf5"
mode = "r+"
groupName = "MVVLVA_TTinRootAndSearch_Killer"
println("Make sure to name \"groupName\" something unique for this build")

h5open(hdf5FileName, mode) do file
	if exists(file, groupName)
		g = g_open(file, groupName)
		o_delete(g, "ordering")
		o_delete(g, "depth")
		o_delete(g, "rawNodes")
		o_delete(g, "leafNodes")
		o_delete(g, "nps")
		o_delete(g, "posType")
		o_delete(g, "gameStage")
		o_delete(g, "time")
	else
		g = g_create(file, groupName)
	end

	g["ordering"] = ordering
	g["depth"] = depth
	g["rawNodes"] = rawNodes
	g["leafNodes"] = leafNodes
	g["nps"] = nps
	g["posType"] = posType
	g["gameStage"] = gameStage
	g["time"] = time
end
