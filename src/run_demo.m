function run_demo(seed)
% RUN_DEMO executes the Phase 4 demo script. This wrapper keeps the entry
% point simple for a user who only wants to run the scenario.

if nargin < 1 || isempty(seed)
    seed = 1;
end

phase4_demo(seed);
end
