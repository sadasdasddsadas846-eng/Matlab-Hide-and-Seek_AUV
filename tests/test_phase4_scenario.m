classdef test_phase4_scenario < matlab.unittest.TestCase
    methods (Test)
        function scenarioRunsAndProducesOutcome(testCase)
            config = phase4_config();
            result = phase4_run_trial(1, config);
            testCase.verifyTrue(isfield(result, 'outcome'));
            testCase.verifyTrue(isfield(result, 'stateA'));
            testCase.verifyTrue(isfield(result, 'stateB'));
            testCase.verifyTrue(isfield(result, 'time'));
            metrics = phase4_metrics(result);
            testCase.verifyTrue(isfield(metrics, 'minSeparation'));
            testCase.verifyGreaterThanOrEqual(metrics.minSeparation, 0.0);
        end

        function sameSeedProducesDeterministicRun(testCase)
            config = phase4_config();
            resultA = phase4_run_trial(7, config);
            resultB = phase4_run_trial(7, config);
            testCase.verifyEqual(resultA.stateA, resultB.stateA, 'AbsTol', 1e-12);
            testCase.verifyEqual(resultA.stateB, resultB.stateB, 'AbsTol', 1e-12);
            testCase.verifyEqual(resultA.time, resultB.time, 'AbsTol', 1e-12);
            testCase.verifyEqual(resultA.detectionCount, resultB.detectionCount);
        end

        function metricsMatchTrajectory(testCase)
            config = phase4_config();
            result = phase4_run_trial(3, config);
            metrics = phase4_metrics(result);
            testCase.verifyEqual(metrics.finalA, result.stateA(end, :), 'AbsTol', 1e-12);
            testCase.verifyEqual(metrics.finalB, result.stateB(end, :), 'AbsTol', 1e-12);
            testCase.verifyGreaterThanOrEqual(metrics.minSeparation, 0.0);
        end

        function finalMetricsMatchLastStateAndAreNotAllZero(testCase)
            config = phase4_config();
            result = phase4_run_trial(9, config);
            metrics = phase4_metrics(result);
            testCase.verifyEqual(metrics.finalA, result.stateA(end, :), 'AbsTol', 1e-12);
            testCase.verifyFalse(all(result.stateA(end, :) == 0, 'all'));
            testCase.verifyFalse(all(result.stateB(end, :) == 0, 'all'));
        end

        function minSeparationMatchesIndependentVectorNorm(testCase)
            config = phase4_config();
            result = phase4_run_trial(11, config);
            metrics = phase4_metrics(result);
            expectedMin = min(vecnorm(result.stateA(:, 1:2) - result.stateB(:, 1:2), 2, 2));
            testCase.verifyEqual(metrics.minSeparation, expectedMin, 'AbsTol', 1e-12);
        end

        function bWinsOnDetectionFlagCanTerminateTheRun(testCase)
            config = phase4_config();
            config.bWinsOnDetection = true;
            config.agentAStart = [0.0; 0.0; 0.0; 0.0; 0.0; 0.0];
            config.agentBStart = [0.5; 0.0; 0.0; 0.0; 0.0; 0.0];
            result = phase4_run_trial(4, config);
            testCase.verifyTrue(strcmp(result.outcome, 'B_WINS') || result.detectionCount > 0);
        end

        function runBatchIsReproducibleWithSeed(testCase)
            config = phase4_config();
            first = run_batch(5, config);
            second = run_batch(5, config);
            testCase.verifyEqual(first.outcomes, second.outcomes);
            testCase.verifyEqual(first.times, second.times, 'AbsTol', 1e-12);
            testCase.verifyEqual(first.detectionCounts, second.detectionCounts);
        end
    end
end
