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

data.mpi_8N_8n = importfile("../../results/ping_pong/mpi_ping_pong_8N_8n.csv");
data.upcxx_rput_as_rpc_8N_8n = importfile("../../results/ping_pong/upcxx_ping_pong_rput_8N_8n.csv");
data.upcxx_rput_no_flag_8N_8n = importfile("../../results/ping_pong/upcxx_ping_pong_rput_no_flag_8N_8n.csv");
data.upcxx_rput_then_8N_8n = importfile("../../results/ping_pong/upcxx_ping_pong_rput_then_8N_8n.csv");
data.upcxx_rput_wait_8N_8n = importfile("../../results/ping_pong/upcxx_ping_pong_rput_wait_8N_8n.csv");
data.upcxx_rpc_8N_8n = importfile("../../results/ping_pong/upcxx_ping_pong_rpc_8N_8n.csv");
data.upcxx_rpc_no_flag_8N_8n = importfile("../../results/ping_pong/upcxx_ping_pong_rpc_no_flag_8N_8n.csv");

data.mpi_4N_8n = importfile("../../results/ping_pong/mpi_ping_pong_4N_8n.csv");
data.upcxx_rput_as_rpc_4N_8n = importfile("../../results/ping_pong/upcxx_ping_pong_rput_4N_8n.csv");
data.upcxx_rput_no_flag_4N_8n = importfile("../../results/ping_pong/upcxx_ping_pong_rput_no_flag_4N_8n.csv");
data.upcxx_rput_then_4N_8n = importfile("../../results/ping_pong/upcxx_ping_pong_rput_then_4N_8n.csv");
data.upcxx_rput_wait_4N_8n = importfile("../../results/ping_pong/upcxx_ping_pong_rput_wait_4N_8n.csv");
data.upcxx_rpc_4N_8n = importfile("../../results/ping_pong/upcxx_ping_pong_rpc_4N_8n.csv");
data.upcxx_rpc_no_flag_4N_8n = importfile("../../results/ping_pong/upcxx_ping_pong_rpc_no_flag_4N_8n.csv");

unique_sizes = unique(data.mpi_8N_8n.BlockSize);
unique_sizes_bytes = unique_sizes .* 4;

% Get field names
fields = fieldnames(data);
fields_8N_8n = (2:5);
fields_4N_8n = (9:12);
fields_rpc_8N_8n = (6:7);
fields_rpc_4N_8n = (13:14);

% Apply function to each field
for i = 1:numel(fields)
    clean_data.(fields{i}) = rmoperblocksize(data.(fields{i}));
end

%% Bandwidth means

bandwidth_data = struct();
bandwidth_mean = struct();
data_mean = struct();

for i = 1:numel(fields)
    bandwidth_data.(fields{i}) = (clean_data.(fields{i}).BlockSize .* int_size) ./ clean_data.(fields{i}).Time;
    bandwidth_mean.(fields{i}) = arrayfun(@(size) ...
        mean(bandwidth_data.(fields{i})(clean_data.(fields{i}).BlockSize == size)), ...
        unique_sizes);
    data_mean.(fields{i}) = arrayfun(@(size) ...
        mean(clean_data.(fields{i}).Time(clean_data.(fields{i}).BlockSize == size)), ...
        unique_sizes);
end

%% Plot utilities

markers = ["o"; "+"; "x";"square"; "diamond"];
formatted_fields = regexprep(fields, "_", "\\_");
size_tick_labels = {'4', '16', '64', '256', '1K', '4K', '16K', '64K', '256K', '512K'};
legend_names = {"as\_rpc", "no\_flag", "then", "wait"};

%% Plot 8N

sizes_figure_8N = figure("Position", figure_position);
plot_objects = gobjects(numel(fields), 1);

hold on

marker_i = 1;
for i = fields_8N_8n
    marker = markers(marker_i);
    marker_i = marker_i + 1;
    plot_objects(i) = plot(unique_sizes_bytes, bandwidth_mean.(fields{i}), "--", "Marker", marker, 'DisplayName', formatted_fields{i});
end

set(gca, "XScale", "log");
set(gca, "YScale", "log");
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
[min_bandwidth, max_bandwidth] = get_min_max(bandwidth_mean, fields(fields_8N_8n));
ax.YTick = get_ytick_range(min_bandwidth, max_bandwidth);
remove_m_ticks();
xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])
lgd = legend(legend_names, "Location","southeast");
%lgd.FontSize = 7;
xlabel('Tamaño (bytes)');
ylabel('Ancho de banda (B/s)');
if (~do_print)
    title('Mean bandwidth in N = 8 n = 8');
end
grid on;

% Create a data cursor object and update its properties
dcm = datacursormode(gcf);
set(dcm, 'UpdateFcn', @updatedcm)

if (do_print)
    print("ping_pong_8N_8n", "-dpng");
end

% Biggest diffs in time and bandwidth
% then_mean = data_mean.upcxx_rput_then_8N_8n(2);
% mpi_mean = data_mean.mpi_8N_8n(2);
% 
% then_mean_b = bandwidth_mean.upcxx_rput_then_8N_8n(2);
% mpi_mean_b = bandwidth_mean.mpi_8N_8n(2);
% 
% diff = ((then_mean - mpi_mean) / mpi_mean) * 100;
% disp(['(8N) rput then is ', int2str(diff), '% faster than mpi']);
% diff = ((then_mean_b - mpi_mean_b) / mpi_mean_b) * 100;
% disp(['(8N) (bandwidth) rput then is ', int2str(diff), '% faster than mpi']);

%% Plot 4N

sizes_figure_4N = figure("Position", figure_position);
plot_objects = gobjects(numel(fields), 1);

hold on

marker_i = 1;
for i = fields_4N_8n
    marker = markers(marker_i);
    marker_i = marker_i + 1;
    plot_objects(i) = plot(unique_sizes_bytes, bandwidth_mean.(fields{i}), "--", "Marker", marker, 'DisplayName', formatted_fields{i});
end

set(gca, "XScale", "log")
set(gca, "YScale", "log")
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
[min_bandwidth, max_bandwidth] = get_min_max(bandwidth_mean, fields(fields_4N_8n));
ax.YTick = get_ytick_range(min_bandwidth, max_bandwidth);
remove_m_ticks();
xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])
lgd = legend(legend_names, "Location","southeast");
%lgd.FontSize = 7;
xlabel('Tamaño (bytes)');
ylabel('Ancho de banda (B/s)');
if (~do_print)
    title('Mean bandwidth in N = 4 n = 8');
end
grid on;

% Create a data cursor object and update its properties
dcm = datacursormode(gcf);
set(dcm, 'UpdateFcn', @updatedcm)

if (do_print)
    print("ping_pong_4N_8n", "-dpng");
end

% % Biggest diffs in time and bandwidth
% wait_mean = data_mean.upcxx_rput_wait_4N_8n(1);
% mpi_mean = data_mean.mpi_4N_8n(1);
% 
% wait_mean_b = bandwidth_mean.upcxx_rput_wait_4N_8n(1);
% mpi_mean_b = bandwidth_mean.mpi_4N_8n(1);
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

%% Plot rpc

sizes_figure_rpc = figure("Position", figure_position);
plot_objects = gobjects(numel(fields), 1);

hold on

marker_i = 1;
for i = fields_rpc_8N_8n
    marker = markers(marker_i);
    marker_i = marker_i + 1;
    plot_objects(i) = plot(unique_sizes_bytes, bandwidth_mean.(fields{i}), "--", "Marker", marker, 'DisplayName', formatted_fields{i});
end

for i = fields_rpc_4N_8n
    marker = markers(marker_i);
    marker_i = marker_i + 1;
    plot_objects(i) = plot(unique_sizes_bytes, bandwidth_mean.(fields{i}), "--", "Marker", marker, 'DisplayName', formatted_fields{i});
end

set(gca, "XScale", "log")
set(gca, "YScale", "log")
xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])
lgd = legend("show", "Location","southeast");
lgd.FontSize = 7;
xlabel('Tamaño (bytes)');
ylabel('Ancho de banda (B/s)');
if (~do_print)
    title('Mean bandwidth in rpcs');
end
grid on;

% Create a data cursor object and update its properties
dcm = datacursormode(gcf);
set(dcm, 'UpdateFcn', @updatedcm)

if (do_print)
    print("ping_pong_rpc", "-dpng");
end

% Diffs between rpc and fastest in 4MB
% then_mean_8N = data_mean.upcxx_rput_then_8N_8n(1);
% rpc_mean_8N = data_mean.upcxx_rpc_8N_8n(1);
% 
% wait_mean_4N = data_mean.upcxx_rput_wait_4N_8n(1);
% rpc_mean_4N = data_mean.upcxx_rpc_4N_8n(1);
% 
% diff = ((rpc_mean_8N - then_mean_8N) / then_mean_8N) * 100;
% disp(['(8N) rpc is ', int2str(diff), '% slower than then']);
% diff = ((rpc_mean_4N - wait_mean_4N) / wait_mean_4N) * 100;
% disp(['(4N) rpc is ', int2str(diff), '% slower than wait']);

%% Plot best upcxx against mpi

mpi_plot = figure("Position", figure_position);
plot_objects = gobjects(numel(fields), 1);

hold on

marker_i = 1;

marker = markers(marker_i);
marker_i = marker_i + 1;
mpi_8N_plot = plot(unique_sizes_bytes, bandwidth_mean.mpi_8N_8n, "--", "Marker", marker, 'DisplayName', formatted_fields{1});

marker = markers(marker_i);
marker_i = marker_i + 1;
upcxx_best_8N_plot = plot(unique_sizes_bytes, bandwidth_mean.upcxx_rput_then_8N_8n, "--", "Marker", marker, 'DisplayName', formatted_fields{4});

marker = markers(marker_i);
marker_i = marker_i + 1;
mpi_4N_plot = plot(unique_sizes_bytes, bandwidth_mean.mpi_4N_8n, "--", "Marker", marker, 'DisplayName', formatted_fields{8});

marker = markers(marker_i);
marker_i = marker_i + 1;
upcxx_best_4N_plot = plot(unique_sizes_bytes, bandwidth_mean.upcxx_rput_wait_4N_8n, "--", "Marker", marker, 'DisplayName', formatted_fields{12});

set(gca, "XScale", "log");
set(gca, "YScale", "log");
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
ax.YTick = 10.^(0:10);
remove_m_ticks();
xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])
legend_names = {"mpi\_8N", "upcxx\_8N", "mpi\_4N", "upcxx\_4N"};
lgd = legend(legend_names, "Location","southeast");
%lgd.FontSize = 7;
xlabel('Tamaño (bytes)');
ylabel('Ancho de banda (B/s)');
if (~do_print)
    title('Mean bandwidth in best upcxx and mpi');
end
grid on;

% Create a data cursor object and update its properties
dcm = datacursormode(gcf);
set(dcm, 'UpdateFcn', @updatedcm)

if (do_print)
    print("ping_pong_best", "-dpng");
end

difference_8N = abs(bandwidth_mean.mpi_8N_8n ./ bandwidth_mean.upcxx_rput_then_8N_8n);
dispmaxdiff('[8N, mpi]', difference_8N, size_tick_labels)

difference_8N_upcxx = abs(bandwidth_mean.upcxx_rput_then_8N_8n ./ bandwidth_mean.mpi_8N_8n);
dispmaxdiff('[8N, upcxx]', difference_8N_upcxx, size_tick_labels)

difference_4N = abs(bandwidth_mean.mpi_4N_8n ./ bandwidth_mean.upcxx_rput_wait_4N_8n);
dispmaxdiff('[4N, mpi]', difference_4N, size_tick_labels)

difference_4N_upcxx = abs(bandwidth_mean.upcxx_rput_wait_4N_8n ./ bandwidth_mean.mpi_4N_8n);
dispmaxdiff('[4N, upcxx]', difference_4N_upcxx, size_tick_labels)