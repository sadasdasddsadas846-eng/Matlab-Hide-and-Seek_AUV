function results = run_batch(N, config)
% RUN_BATCH executes multiple seeded Phase 4 trials and returns a structured
% summary for plotting or reporting.

if nargin < 2 || isempty(config)
    config = phase4_config();
end
if nargin < 1 || isempty(N)
    N = 10;
end

results = struct();
results.N = N;
results.outcomes = strings(N, 1);
results.times = zeros(N, 1);
results.detectionCounts = zeros(N, 1);
results.minSeparations = zeros(N, 1);

for i = 1:N
    trial = phase4_run_trial(i, config);
    metrics = phase4_metrics(trial);
    results.outcomes(i) = string(metrics.result);
    results.times(i) = metrics.time;
    results.detectionCounts(i) = metrics.detectionCount;
    results.minSeparations(i) = metrics.minSeparation;
end
end
