classdef test_phase2b < matlab.unittest.TestCase
    % TEST_PHASE2B validates that the Simulink two-agent wrapper reproduces the
    % plain MATLAB reference result from Phase 2A.

    methods (TestClassSetup)
        function addSrcToPath(testCase)
            repoRoot = fileparts(fileparts(mfilename('fullpath')));
            testCase.applyFixture(matlab.unittest.fixtures.PathFixture(fullfile(repoRoot, 'src')));
        end
    end

    methods (Test)
        function matchesPlainMATLABReference(testCase)
            % I chose +/- 0.02 m because the fixed-step solver uses dt = 0.05 s,
            % and the simulation is deterministic. A 2 cm tolerance is strict enough
            % to catch modeling drift but loose enough to allow trivial numeric
            % differences between the plain MATLAB and Simulink integration paths.
            results = build_phase2b_model();

            refA = [19.56, 0.00];
            refB = [8.00, 21.69];

            testCase.verifyLessThanOrEqual(abs(results.finalA(1) - refA(1)), 0.02);
            testCase.verifyLessThanOrEqual(abs(results.finalA(2) - refA(2)), 0.02);
            testCase.verifyLessThanOrEqual(abs(results.finalB(1) - refB(1)), 0.02);
            testCase.verifyLessThanOrEqual(abs(results.finalB(2) - refB(2)), 0.02);
        end
    end
end
