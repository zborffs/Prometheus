include("CloakTools.jl/CloakTools.jl")
using StatsBase, Plots, PrettyTables, StatsPlots
plotly()

testSuiteData = CloakTools.test_suite_data("Bratko-Kopec", "data/kaufman.txt")

positionData = testSuiteData.positionData

ordering = Vector{Float64}(undef, 0)
depth = Vector{Float64}(undef, 0)
rawNodes = Vector{Float64}(undef, 0)
leafNodes = Vector{Float64}(undef, 0)
nps = Vector{Float64}(undef, 0)
posType = Vector{Float64}(undef, 0)
gameStage = Vector{Float64}(undef, 0)
passed = 0

for i in range(1, length=length(positionData))
    append!(ordering, positionData[i].ordering)
    append!(depth, positionData[i].depth)
    append!(rawNodes, positionData[i].rawNodes)
    append!(leafNodes, positionData[i].leafNodes)
    append!(nps, positionData[i].nps)
    append!(posType, positionData[i].posType)
    append!(gameStage, positionData[i].gameStage)
    if positionData[i].testStatus == CloakTools.Passed
        passed += 1
    end
end

sumDataOrdering = CloakTools.summaryData("Bratko-Kopec Ordering", ordering)
sumDataRawNodes = CloakTools.summaryData("Bratko-Kopec Raw Nodes", rawNodes)
sumDataLeafNodes = CloakTools.summaryData("Bratko-Kopec Leaf Nodes", leafNodes)
sumDataNPS = CloakTools.summaryData("Bratko-Kopec NPS", nps)
sumData = vcat(sumDataOrdering, sumDataRawNodes, sumDataLeafNodes, sumDataNPS)
pretty_table(sumData, CloakTools.TABLE_HEADER)

histogram(ordering)
histogram(rawNodes)
histogram(leafNodes)
histogram(nps)
histogram(posType)
histogram(gameStage)
boxplot(ordering)
