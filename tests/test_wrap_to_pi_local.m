classdef test_wrap_to_pi_local < matlab.unittest.TestCase
    methods (Test)
        function matchesWrapToPi(testCase)
            angles = [ -3*pi, -pi, 0, pi, 3*pi, randn(1, 20) * 10 ];
            for i = 1:numel(angles)
                expected = wrapToPi(angles(i));
                actual = wrap_to_pi_local(angles(i));
                testCase.verifyEqual(actual, expected, 'AbsTol', 1e-12);
            end
        end
    end
end
