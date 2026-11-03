function set_mlfcn_script(blockPath, codeString)
% SET_MLFNC_SCRIPT sets a Simulink MATLAB Function block code using the
% verified R2025b API:
%   rt = sfroot;
%   chart = rt.find('-isa', 'Stateflow.EMChart', 'Path', blockPath);
%   chart.Script = codeString;
%
% This method was verified in MATLAB R2025b by the user and reproduced here.
% If the block path is wrong or the chart is not found, the function throws a
% clear error instead of guessing silently.

if nargin < 2 || isempty(blockPath) || isempty(codeString)
    error('set_mlfcn_script:BadInput', 'blockPath and codeString are required.');
end

if ~contains(codeString, newline)
    error('set_mlfcn_script:NoNewline', ...
        'codeString must contain a real newline character. Use sprintf or strjoin(lines, newline) so the MATLAB Function block can reconcile its ports correctly.');
end

parts = strsplit(codeString, newline, 'CollapseDelimiters', false);
line1 = strtrim(parts{1});
if isempty(line1) || ~startsWith(line1, 'function')
    error('set_mlfcn_script:BadFunctionLine', ...
        'codeString must begin with a valid function line, but the first line was "%s".', line1);
end

rt = sfroot;
chart = rt.find('-isa', 'Stateflow.EMChart', 'Path', blockPath);
if isempty(chart)
    error('set_mlfcn_script:ChartNotFound', ...
        'MATLAB Function block chart not found at "%s". Use the verified R2025b method: sfroot; rt.find(''-isa'', ''Stateflow.EMChart'', ''Path'', blockPath);', blockPath);
end

chart.Script = codeString;
updatedScript = chart.Script;
if isempty(updatedScript) || ~startsWith(strtrim(updatedScript), line1)
    error('set_mlfcn_script:ScriptNotSet', ...
        'MATLAB Function block script was not set correctly at "%s". Expected it to begin with "%s".', blockPath, line1);
end
end
