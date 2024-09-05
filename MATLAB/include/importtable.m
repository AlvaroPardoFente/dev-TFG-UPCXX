function output_table = importtable(filename, dataLines)
%IMPORTFILE Import data from a text file
%  OUTPUT_TABLE = IMPORTTABLE(FILENAME) reads data from text file
%  FILENAME for the default selection.  Returns the data as a table.
%
%  OUTPUT_TABLE = IMPORTTABLE(FILE, DATALINES) reads data for the
%  specified row interval(s) of text file FILENAME. Specify DATALINES as
%  a positive scalar integer or a N-by-2 array of positive scalar
%  integers for dis-contiguous row intervals.
%
%  Example:
%  mpibroadcastarray = importfile("mpi_broadcast_array.csv", [3, Inf]);
%
%  See also READTABLE.
%
% Auto-generated by MATLAB on 22-May-2024 16:51:54

%% Input handling

% If dataLines is not specified, define defaults
if nargin < 2
    dataLines = [4, Inf];
end

%% Set up the Import Options and import the data
opts = delimitedTextImportOptions("NumVariables", 5);

% Specify range and delimiter
opts.DataLines = dataLines;
opts.Delimiter = ",";

% Specify column names and types
opts.VariableNames = ["Rank", "Size", "Timepoint", "Index", "Time"];
opts.VariableTypes = ["double", "double", "categorical", "double", "double"];

% Specify file level properties
opts.ExtraColumnsRule = "ignore";
opts.EmptyLineRule = "read";

% Specify variable properties
opts = setvaropts(opts, "Timepoint", "EmptyFieldRule", "auto");

% Import the data
output_table = readtable(filename, opts);

end