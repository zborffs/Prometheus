include("src/CloakTools.jl")


#
# # this script opens bartko.txt, a file containing the engine's output for the
# # Bartko-Kopec test suite, and analyzes the output
#
# using Plots, Statistics, Distributions, StatsPlots, ColorSchemes, PrettyTables, StatsBase
# plotly()
#
# headers = ["Statistic", "N", "Mean", "Min", "25 Percentile", "Median", "75 Percentile", "Max", "Standard Deviation", "Skewness", "Kurtosis"]
#
# overallOrdering = summaryData(engData.ordering)
# # overallDepth = summaryData(engData.depth)
# # overallDepth = summaryData(engData.rawNodes)
# # overallDepth = summaryData(engData.leafNodes)
# # overallDepth = summaryData(engData.nps)
# # overallDepth = summaryData(engData.posType)
#
# function printTestSuite(testSuiteData, engData)
# 	clearconsole()
# 	n = testSuiteData.num_games
# 	k = testSuiteData.num_passed
#
# 	title = string("Test Suite: ", testSuiteData.name)
# 	thickLine = repeat('=', length(title))
# 	line = repeat('-', length(title))
# 	headers = ["Statistic" "N" "Mean" "Min" "25 Percentile" "Median" "75 Percentile" "Max" "Standard Deviation" "Skewness" "Kurtosis"]
# 	overallOrdering = summaryData(engData.ordering, "Overall Ordering")
# 	overallTime = summaryData(engData.time, "Overall Time")
#
# 	# concat data for printing
# 	data = vcat(overallOrdering, overallTime)
#
# 	println(title)
# 	println(thickLine)
#
# 	pretty_table(data, headers)
# end
#
# # Read raw data from file
#
#
# engData, testSuiteData = set_data("Bratko-Kopec", lines)
#
# printTestSuite(testSuiteData, engData)
#
# # Print the data to the screen for the user
# println("N (Number of Games) = $(testSuiteData.num_games)")
