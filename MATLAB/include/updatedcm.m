function output_txt = updatedcm(~, event)
%UPDATEDCM Add the DisplayName to the cursor display

% Extract the data index and the x and y values
idx = event.DataIndex;
x = event.Position(1);
y = event.Position(2);
% Extract the legend name for the corresponding series
leg = event.Target.DisplayName;
% Create a cell array with custom display text
output_txt = {['Series: ', leg], ['X: ', num2str(x)], ['Y: ', num2str(y)]};
end