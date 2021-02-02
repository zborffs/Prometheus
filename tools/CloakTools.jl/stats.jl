using StatsBase

TABLE_HEADER = ["Statistic", "N", "Mean", "Min", "25 Percentile", "Median", "75 Percentile", "Max", "Standard Deviation", "Skewness", "Kurtosis"]

function summaryData(name, data)
	n = length(data)
	μ, σ = mean_and_std(data)
	min, q1, med, q3, max = StatsBase.nquantile(data, 4)
	skewness = StatsBase.skewness(data)
	kurtosis = StatsBase.kurtosis(data)
	return [name n μ min q1 med q3 max σ skewness kurtosis]
end
