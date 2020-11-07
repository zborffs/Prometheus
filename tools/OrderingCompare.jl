# This script reads an hdf5 file with parsed data created by "Ordering.jl" and
# compares it.


# Import Packages
using Plots, Statistics, Distributions, StatsPlots, HDF5, ColorSchemes
plotly()

# Try to read the same data back
hdf5FileName = "data/ParsedOrdering.hdf5"
groupNames = ["NoOrdering", "MVVLVA"]
readMode = "r"
ordering = Vector{Vector{Float64}}(undef, length(groupNames))
depth = Vector{Vector{Int64}}(undef, length(groupNames))
rawNodes = Vector{Vector{Int64}}(undef, length(groupNames))
leafNodes = Vector{Vector{Int64}}(undef, length(groupNames))
nps = Vector{Vector{Float64}}(undef, length(groupNames))
posType = Vector{Vector{Int64}}(undef, length(groupNames))
gameStage = Vector{Vector{Int64}}(undef, length(groupNames))
time = Vector{Vector{Float64}}(undef, length(groupNames))

for i in 1:length(ordering)
	ordering[i] = h5open(hdf5FileName, readMode) do file
		h5read(hdf5FileName, string(groupNames[i], "/ordering"))
	end

	depth[i] = h5open(hdf5FileName, readMode) do file
		h5read(hdf5FileName, string(groupNames[i], "/depth"))
	end

	rawNodes[i] = h5open(hdf5FileName, readMode) do file
		h5read(hdf5FileName, string(groupNames[i], "/rawNodes"))
	end

	leafNodes[i] = h5open(hdf5FileName, readMode) do file
		h5read(hdf5FileName, string(groupNames[i], "/leafNodes"))
	end

	nps[i] = h5open(hdf5FileName, readMode) do file
		h5read(hdf5FileName, string(groupNames[i], "/nps"))
	end

	posType[i] = h5open(hdf5FileName, readMode) do file
		h5read(hdf5FileName, string(groupNames[i], "/posType"))
	end

	gameStage[i] = h5open(hdf5FileName, readMode) do file
		h5read(hdf5FileName, string(groupNames[i], "/gameStage"))
	end

	time[i] = h5open(hdf5FileName, readMode) do file
		h5read(hdf5FileName, string(groupNames[i], "/time"))
	end
end

@assert length(ordering[1]) == length(ordering[2])

# Overall Move Ordering Comparison
histogram(ordering[1], bins=16, label="No Ordering", title="Move Ordering Scheme Comparisons", xlabel="Move Ordering (%)", ylabel="Frequency (# Occurrences)", palette=cgrad(ColorSchemes.berlin.colors; alpha=0.33))
histogram!(ordering[2], bins=16, label="MVVLVA")
overallMean = Vector{Float64}(undef, length(ordering))
overallStd = Vector{Float64}(undef, length(ordering))
overallMedian = Vector{Float64}(undef, length(ordering))
for i in 1:length(ordering)
	overallMean[i] = mean(ordering)
	overallStd[i] = std(ordering)
	overallMedian[i] = median(ordering)
end

# Opening Move Ordering Comparison
openingIndices = union(findall(x->x == 1, gameStage[1]), findall(x->x == 1, gameStage[2]))
openingOrdering = Vector{Vector{Float64}}(undef, length(ordering))
for i in 1:length(openingOrdering)
	openingOrdering[i] = ordering[i][openingIndices]
end
openingMean = Vector{Float64}(undef, length(openingOrdering))
openingStd = Vector{Float64}(undef, length(openingOrdering))
openingMedian = Vector{Float64}(undef, length(openingOrdering))
for i in 1:length(openingOrdering)
	openingMean[i] = mean(openingOrdering[i])
	openingStd[i] = std(openingOrdering[i])
	openingMedian[i] = median(openingOrdering[i])
end
histogram(openingOrdering[1], label="No Ordering", title="Opening Move Ordering Scheme Comparisons", xlabel="Move Ordering (%)", ylabel="Frequency (# Occurrences)", palette=cgrad(ColorSchemes.berlin.colors; alpha=0.33))
histogram!(openingOrdering[2], label="MVVLVA")

# Middle Game
middleIndices = union(findall(x->x == 2, gameStage[1]), findall(x->x == 2, gameStage[2]))
middleOrdering = Vector{Vector{Float64}}(undef, length(ordering))
for i in 1:length(middleOrdering)
	middleOrdering[i] = ordering[i][middleIndices]
end
middleMean = Vector{Float64}(undef, length(middleOrdering))
middleStd = Vector{Float64}(undef, length(middleOrdering))
middleMedian = Vector{Float64}(undef, length(middleOrdering))
for i in 1:length(middleOrdering)
	middleMean[i] = mean(middleOrdering[i])
	middleStd[i] = std(middleOrdering[i])
	middleMedian[i] = median(middleOrdering[i])
end
histogram(middleOrdering[1], bins=12, label="No Ordering", title="Mid-Game Move Ordering Scheme Comparisons", xlabel="Move Ordering (%)", ylabel="Frequency (# Occurrences)", palette=cgrad(ColorSchemes.berlin.colors; alpha=0.33))
histogram!(middleOrdering[2], bins=12, label="MVVLVA")

# Early End Game
earlyendIndices = union(findall(x->x == 3, gameStage[1]), findall(x->x == 3, gameStage[2]))
earlyendOrdering = Vector{Vector{Float64}}(undef, length(ordering))
for i in 1:length(earlyendOrdering)
	earlyendOrdering[i] = ordering[i][earlyendIndices]
end
earlyendMean = Vector{Float64}(undef, length(earlyendOrdering))
earlyendStd = Vector{Float64}(undef, length(earlyendOrdering))
earlyendMedian = Vector{Float64}(undef, length(earlyendOrdering))
for i in 1:length(earlyendOrdering)
	earlyendMean[i] = mean(earlyendOrdering[i])
	earlyendStd[i] = std(earlyendOrdering[i])
	earlyendMedian[i] = median(earlyendOrdering[i])
end
histogram(earlyendOrdering[1], bins=12, label="No Ordering", title="Early End-Game Move Ordering<br> Scheme Comparisons", xlabel="Move Ordering (%)", ylabel="Frequency (# Occurrences)", palette=cgrad(ColorSchemes.berlin.colors; alpha=0.33))
histogram!(earlyendOrdering[2], bins=12, label="MVVLVA")

# Late End Game
lateendIndices = union(findall(x->x == 4, gameStage[1]), findall(x->x == 4, gameStage[2]))
lateendOrdering = Vector{Vector{Float64}}(undef, length(ordering))
for i in 1:length(lateendOrdering)
	lateendOrdering[i] = ordering[i][lateendIndices]
end
lateendMean = Vector{Float64}(undef, length(lateendOrdering))
lateendStd = Vector{Float64}(undef, length(lateendOrdering))
lateendMedian = Vector{Float64}(undef, length(lateendOrdering))
for i in 1:length(lateendOrdering)
	lateendMean[i] = mean(lateendOrdering[i])
	lateendStd[i] = std(lateendOrdering[i])
	lateendMedian[i] = median(lateendOrdering[i])
end
histogram(lateendOrdering[1], bins=12, label="No Ordering", title="Late End-Game Move Ordering<br> Scheme Comparisons", xlabel="Move Ordering (%)", ylabel="Frequency (# Occurrences)", palette=cgrad(ColorSchemes.berlin.colors; alpha=0.33))
histogram!(lateendOrdering[2], bins=12, label="MVVLVA")
