function [output_table] = rmopersizeperrank(input_table, varargin)
%RMOPERSIZE Removes time outliers grouped by a specified column
%   A table with various columns is passed. The function divides
%   the data based on the specified column (default is Size) and removes
%   outliers based on Time

% Default column to group by
group_column = 'Sizeperrank';

% Check if a second argument is provided
if ~isempty(varargin)
    group_column = varargin{1};
end

% Initialize cleaned_data with the same structure as input_table
cleaned_data = input_table([],:);

% Get unique values of the group column
unique_groups = unique(input_table.(group_column));

% Loop over each group
for i = 1:length(unique_groups)
    group_value = unique_groups(i);
    % Extract the group data
    group_data = input_table(input_table.(group_column) == group_value, :);
    
    % Remove outliers from the 'Time' column
    [group_data_cleaned, TF] = rmoutliers(group_data, 'DataVariables', 'Time');
    
    % Append the cleaned group data to the cleaned_data table
    cleaned_data = [cleaned_data; group_data_cleaned];
end

output_table = cleaned_data;

end