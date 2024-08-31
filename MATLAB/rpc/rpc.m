%% Add common functions to the path

clearvars;
addpath("../include/")

%% ----------------------PRINTING----------------------

do_print = false;

%% Import and process data

int_size = 4;

figure_position = [680 458 480 240];

data = struct();
clean_data = struct();

data.mpi_rpc_2N_2n = importtable("../../results/rpc/mpi_rpc_2N_2n.csv");
data.upcxx_rpc_notice_2N_2n = importtable("../../results/rpc/upcxx_rpc_notice_2N_2n.csv");
data.upcxx_rpc_ff_notice_2N_2n = importtable("../../results/rpc/upcxx_rpc_ff_notice_2N_2n.csv");
data.upcxx_rpc_then_2N_2n = importtable("../../results/rpc/upcxx_rpc_then_2N_2n.csv");
data.upcxx_rpc_vector_2N_2n = importtable("../../results/rpc/upcxx_rpc_vector_2N_2n.csv");

data.mpi_rpc_1N_2n = importtable("../../results/rpc/mpi_rpc_1N_2n.csv");
data.upcxx_rpc_notice_1N_2n = importtable("../../results/rpc/upcxx_rpc_notice_1N_2n.csv");
data.upcxx_rpc_ff_notice_1N_2n = importtable("../../results/rpc/upcxx_rpc_ff_notice_1N_2n.csv");
data.upcxx_rpc_then_1N_2n = importtable("../../results/rpc/upcxx_rpc_then_1N_2n.csv");
data.upcxx_rpc_vector_1N_2n = importtable("../../results/rpc/upcxx_rpc_vector_1N_2n.csv");

unique_sizes = unique(data.mpi_rpc_2N_2n.Size);
unique_sizes_bytes = unique_sizes;

% Get field names
fields = fieldnames(data);
fields_2N_2n = (2:3);
fields_1N_2n = (7:8);
fields_return_2N_2n = (4:5);
fields_return_1N_2n = (9:10);

% Apply function to each field
for i = 1:numel(fields)
    clean_data.(fields{i}) = rmopersize(data.(fields{i}));
end

%% Time means

data_mean = struct();

for i = 1:numel(fields)
    data_mean.(fields{i}) = arrayfun(@(size) ...
        mean(clean_data.(fields{i}).Time(clean_data.(fields{i}).Size == size)), ...
        unique_sizes);
end

%% Plot utilities

markers = ["o"; "+"; "x";"square"; "diamond"];
formatted_fields = regexprep(fields, "_", "\\_");
size_tick_labels = {'1', '4', '16', '64', '256', '1K', '4K', '16K', '64K', '128K'};
legend_names = {"rpc\_notice", "rpc\_ff"};

%% Plot upcxx 2N

sizes_figure_2N = figure("Position", figure_position);
plot_objects = gobjects(numel(fields), 1);

hold on

marker_i = 1;
for i = fields_2N_2n
    marker = markers(marker_i);
    marker_i = marker_i + 1;
    plot_objects(i) = plot(unique_sizes_bytes, data_mean.(fields{i}), "--", "Marker", marker, 'DisplayName', formatted_fields{i});
end

set(gca, "XScale", "log");
set(gca, "YScale", "log");
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
[min_time, max_time] = get_min_max(data_mean, fields(fields_2N_2n));
remove_m_ticks();
xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])
lgd = legend(legend_names, "Location","southeast");
%lgd.FontSize = 7;
xlabel('Iterations');
ylabel('Time (s)');
if (~do_print)
    title('Mean time for upcxx with no return in N = 2 n = 2');
end
grid on;

% Create a data cursor object and update its properties
dcm = datacursormode(gcf);
set(dcm, 'UpdateFcn', @updatedcm)

if (do_print)
    print("upcxx_rpc_2N_2n", "-dpng");
end

% Biggest diffs in time and bandwidth
% then_mean = data_mean.upcxx_rput_then_8N_8n(2);
% mpi_mean = data_mean.mpi_rpc_2N_2n(2);
% 
% then_mean_b = bandwidth_mean.upcxx_rput_then_8N_8n(2);
% mpi_mean_b = bandwidth_mean.mpi_rpc_2N_2n(2);
% 
% diff = ((then_mean - mpi_mean) / mpi_mean) * 100;
% disp(['(8N) rput then is ', int2str(diff), '% faster than mpi']);
% diff = ((then_mean_b - mpi_mean_b) / mpi_mean_b) * 100;
% disp(['(8N) (bandwidth) rput then is ', int2str(diff), '% faster than mpi']);

%% Plot upcxx 1N

sizes_figure_4N = figure("Position", figure_position);
plot_objects = gobjects(numel(fields), 1);

hold on

marker_i = 1;
for i = fields_1N_2n
    marker = markers(marker_i);
    marker_i = marker_i + 1;
    plot_objects(i) = plot(unique_sizes_bytes, data_mean.(fields{i}), "--", "Marker", marker, 'DisplayName', formatted_fields{i});
end

set(gca, "XScale", "log")
set(gca, "YScale", "log")
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
[min_time, max_time] = get_min_max(data_mean, fields(fields_1N_2n));
% ax.YTick = get_ytick_range(min_bandwidth, max_bandwidth);
remove_m_ticks();
xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])
lgd = legend(legend_names, "Location","southeast");
%lgd.FontSize = 7;
xlabel('Iterations');
ylabel('Time (s)');
if (~do_print)
    title('Mean bandwidth for upcxx with no return in N = 1 n = 2');
end
grid on;

% Create a data cursor object and update its properties
dcm = datacursormode(gcf);
set(dcm, 'UpdateFcn', @updatedcm)

if (do_print)
    print("upcxx_rpc_1N_2n", "-dpng");
end

% % Biggest diffs in time and bandwidth
% wait_mean = data_mean.upcxx_rput_wait_4N_8n(1);
% mpi_mean = data_mean.mpi_rpc_1N_2n(1);
% 
% wait_mean_b = bandwidth_mean.upcxx_rput_wait_4N_8n(1);
% mpi_mean_b = bandwidth_mean.mpi_rpc_1N_2n(1);
% 
% diff = ((wait_mean - mpi_mean) / mpi_mean) * 100;
% disp(['(4N) rput wait is ', int2str(diff), '% faster than mpi']);
% diff = ((wait_mean_b - mpi_mean_b) / mpi_mean_b) * 100;
% disp(['(4N) (bandwidth) rput wait is ', int2str(diff), '% faster than mpi']);
% 
% % Diffs size=1 in fastest 8N and 4N
% then_mean_8N = data_mean.upcxx_rput_then_8N_8n(1);
% wait_mean_4N = data_mean.upcxx_rput_wait_4N_8n(1);
% diff = ((wait_mean_4N - then_mean_8N) / then_mean_8N) * 100;
% disp(['(1) 4N wait is ', int2str(diff), '% faster than 8N then']);

%% Plot upcxx against mpi on no return

mpi_plot_no_return = figure("Position", figure_position);
plot_objects = gobjects(numel(fields), 1);

hold on

marker_i = 1;

marker = markers(marker_i);
marker_i = marker_i + 1;
mpi_2N_plot = plot(unique_sizes_bytes, data_mean.mpi_rpc_2N_2n, "--", "Marker", marker, 'DisplayName', formatted_fields{1});

marker = markers(marker_i);
marker_i = marker_i + 1;
upcxx_best_2N_plot = plot(unique_sizes_bytes, data_mean.upcxx_rpc_ff_notice_2N_2n, "--", "Marker", marker, 'DisplayName', formatted_fields{3});

marker = markers(marker_i);
marker_i = marker_i + 1;
mpi_1N_plot = plot(unique_sizes_bytes, data_mean.mpi_rpc_1N_2n, "--", "Marker", marker, 'DisplayName', formatted_fields{6});

marker = markers(marker_i);
marker_i = marker_i + 1;
upcxx_best_1N_plot = plot(unique_sizes_bytes, data_mean.upcxx_rpc_ff_notice_1N_2n, "--", "Marker", marker, 'DisplayName', formatted_fields{8});

set(gca, "XScale", "log");
set(gca, "YScale", "log");
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
ax.YTick = 10.^(0:10);
remove_m_ticks();
xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])
legend_names = {"mpi\_2N", "upcxx\_2N", "mpi\_1N", "upcxx\_1N"};
lgd = legend(legend_names, "Location","northwest");
% lgd.FontSize = 7;
xlabel('Iterations');
ylabel('Time (s)');
if (~do_print)
    title('Mean time in best upcxx and mpi on no return');
end
grid on;

% Create a data cursor object and update its properties
dcm = datacursormode(gcf);
set(dcm, 'UpdateFcn', @updatedcm)

if (do_print)
    print("rpc_best", "-dpng");
end

difference_2N = abs(data_mean.mpi_rpc_2N_2n ./ data_mean.upcxx_rpc_ff_notice_2N_2n);
dispmaxdiff('[8N, mpi]', difference_2N, size_tick_labels)

difference_2N_upcxx = abs(data_mean.upcxx_rpc_ff_notice_2N_2n ./ data_mean.mpi_rpc_2N_2n);
dispmaxdiff('[8N, upcxx]', difference_2N_upcxx, size_tick_labels)

difference_1N = abs(data_mean.mpi_rpc_1N_2n ./ data_mean.upcxx_rpc_ff_notice_1N_2n);
dispmaxdiff('[4N, mpi]', difference_1N, size_tick_labels)

difference_1N_upcxx = abs(data_mean.upcxx_rpc_ff_notice_1N_2n ./ data_mean.mpi_rpc_1N_2n);
dispmaxdiff('[4N, upcxx]', difference_1N_upcxx, size_tick_labels)

%% Plot upcxx return 2N

sizes_figure_return_2N = figure("Position", figure_position);
plot_objects = gobjects(numel(fields), 1);

hold on

marker_i = 1;
for i = fields_return_2N_2n
    marker = markers(marker_i);
    marker_i = marker_i + 1;
    plot_objects(i) = plot(unique_sizes_bytes, data_mean.(fields{i}), "--", "Marker", marker, 'DisplayName', formatted_fields{i});
end

set(gca, "XScale", "log");
set(gca, "YScale", "log");
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
[min_time, max_time] = get_min_max(data_mean, fields(fields_return_2N_2n));
% ax.YTick = get_ytick_range(min_bandwidth, max_bandwidth);
remove_m_ticks();
xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])
legend_names = {"then", "vector"};
lgd = legend(legend_names, "Location","northwest");
%lgd.FontSize = 7;
xlabel('Iterations');
ylabel('Time (s)');
if (~do_print)
    title('Mean time for upcxx with return in N = 2 n = 2');
end
grid on;

% Create a data cursor object and update its properties
dcm = datacursormode(gcf);
set(dcm, 'UpdateFcn', @updatedcm)

if (do_print)
    print("upcxx_rpc_return_2N_2n", "-dpng");
end

%% Plot upcxx return 1N

sizes_figure_return_1N = figure("Position", figure_position);
plot_objects = gobjects(numel(fields), 1);

hold on

marker_i = 1;
for i = fields_return_1N_2n
    marker = markers(marker_i);
    marker_i = marker_i + 1;
    plot_objects(i) = plot(unique_sizes_bytes, data_mean.(fields{i}), "--", "Marker", marker, 'DisplayName', formatted_fields{i});
end

set(gca, "XScale", "log");
set(gca, "YScale", "log");
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
[min_time, max_time] = get_min_max(data_mean, fields(fields_return_1N_2n));
% ax.YTick = get_ytick_range(min_bandwidth, max_bandwidth);
remove_m_ticks();
xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])
legend_names = {"then", "vector"};
lgd = legend(legend_names, "Location","northwest");
%lgd.FontSize = 7;
xlabel('Iterations');
ylabel('Time (s)');
if (~do_print)
    title('Mean time for upcxx with return in N = 1 n = 2');
end
grid on;

% Create a data cursor object and update its properties
dcm = datacursormode(gcf);
set(dcm, 'UpdateFcn', @updatedcm)

if (do_print)
    print("upcxx_rpc_return_1N_2n", "-dpng");
end

%% Plot upcxx return all

sizes_figure_return_all = figure("Position", figure_position);
plot_objects = gobjects(numel(fields), 1);

hold on

marker_i = 1;
for i = fields_return_2N_2n
    marker = markers(marker_i);
    marker_i = marker_i + 1;
    plot_objects(i) = plot(unique_sizes_bytes, data_mean.(fields{i}), "--", "Marker", marker, 'DisplayName', formatted_fields{i});
end

for i = fields_return_1N_2n
    marker = markers(marker_i);
    marker_i = marker_i + 1;
    plot_objects(i) = plot(unique_sizes_bytes, data_mean.(fields{i}), "--", "Marker", marker, 'DisplayName', formatted_fields{i});
end

set(gca, "XScale", "log");
set(gca, "YScale", "log");
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
[min_time, max_time] = get_min_max(data_mean, fields(fields_return_2N_2n));
% ax.YTick = get_ytick_range(min_bandwidth, max_bandwidth);
remove_m_ticks();
xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])
legend_names = {"then\_2N", "vector\_2N", "then\_1N", "vector\_1N"};
lgd = legend(legend_names, "Location", "northwest");
lgd.FontSize = 7;
xlabel('Iterations');
ylabel('Time (s)');
if (~do_print)
    title('Mean bandwidth for upcxx with return in N = 1 n = 2');
end
grid on;

% Create a data cursor object and update its properties
dcm = datacursormode(gcf);
set(dcm, 'UpdateFcn', @updatedcm)

if (do_print)
    print("upcxx_rpc_return_all", "-dpng");
end

difference_2N = abs(data_mean.upcxx_rpc_then_2N_2n ./ data_mean.upcxx_rpc_vector_2N_2n);
dispmaxdiff('[2N, then]', difference_2N, size_tick_labels)

difference_2N_upcxx = abs(data_mean.upcxx_rpc_vector_2N_2n ./ data_mean.upcxx_rpc_then_2N_2n);
dispmaxdiff('[2N, vector]', difference_2N_upcxx, size_tick_labels)

difference_1N = abs(data_mean.upcxx_rpc_then_1N_2n ./ data_mean.upcxx_rpc_vector_1N_2n);
dispmaxdiff('[1N, then]', difference_1N, size_tick_labels)

difference_1N_upcxx = abs(data_mean.upcxx_rpc_vector_1N_2n ./ data_mean.upcxx_rpc_then_1N_2n);
dispmaxdiff('[1N, vector]', difference_1N_upcxx, size_tick_labels)