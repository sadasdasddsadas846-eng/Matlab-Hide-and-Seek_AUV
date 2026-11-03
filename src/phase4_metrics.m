function metrics = phase4_metrics(result)
% PHASE4_METRICS returns a compact summary for a single Phase 4 run.

if nargin < 1 || isempty(result)
    error('phase4_metrics:InvalidInput', 'A result struct is required.');
end

trajA = result.stateA;
trajB = result.stateB;

minSep = inf;
for i = 1:size(trajA, 1)
    sep = norm(trajA(i, 1:2) - trajB(i, 1:2));
    if sep < minSep
        minSep = sep;
    end
end

metrics = struct();
metrics.result = result.outcome;
metrics.time = result.time;
metrics.minSeparation = minSep;
metrics.detectionCount = result.detectionCount;
metrics.finalA = trajA(end, :);
metrics.finalB = trajB(end, :);
metrics.goalReached = result.outcome == "A_WINS";
metrics.captureReached = result.outcome == "B_WINS";
end
