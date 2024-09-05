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

base_path = "../../results/remote_completion/";

data.global_var_1N_2n = importtable(base_path + "upcxx_rpc_global_var_1N_2n");
data.global_var_2N_4n = importtable(base_path + "upcxx_rpc_global_var_2N_4n");
data.global_var_4N_8n = importtable(base_path + "upcxx_rpc_global_var_4N_8n");
data.global_var_6N_12n = importtable(base_path + "upcxx_rpc_global_var_6N_12n");
data.global_var_8N_16n = importtable(base_path + "upcxx_rpc_global_var_8N_16n");
data.global_var_10N_20n = importtable(base_path + "upcxx_rpc_global_var_10N_20n");
data.global_var_12N_24n = importtable(base_path + "upcxx_rpc_global_var_12N_24n");

data.dist_object_1N_2n = importtable(base_path + "upcxx_rpc_dist_object_1N_2n");
data.dist_object_2N_4n = importtable(base_path + "upcxx_rpc_dist_object_2N_4n");
data.dist_object_4N_8n = importtable(base_path + "upcxx_rpc_dist_object_4N_8n");
data.dist_object_6N_12n = importtable(base_path + "upcxx_rpc_dist_object_6N_12n");
data.dist_object_8N_16n = importtable(base_path + "upcxx_rpc_dist_object_8N_16n");
data.dist_object_10N_20n = importtable(base_path + "upcxx_rpc_dist_object_10N_20n");
data.dist_object_12N_24n = importtable(base_path + "upcxx_rpc_dist_object_12N_24n");

unique_sizes = unique(data.global_var_1N_2n.Size);
unique_sizes_bytes = unique_sizes;
unique_sizes_categorical = categorical(unique_sizes);

num_processes = [2; 4; 8; 12; 16; 20; 24];

% Get field names
fields = fieldnames(data);

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
size_tick_labels = {'1', '4', '16', '64', '256', '1K', '4K', '16K'};

%% 2 processes

sizes_figure_2n = figure("Position", figure_position);

plot(unique_sizes_bytes, data_mean.global_var_1N_2n, "--o")
hold on
plot(unique_sizes_bytes, data_mean.dist_object_1N_2n, "--x")
set(gca, "XScale", "log")
set(gca, "YScale", "log")
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
min_time = min(min([data_mean.global_var_1N_2n, data_mean.dist_object_1N_2n]));
max_time = max(max([data_mean.global_var_1N_2n, data_mean.dist_object_1N_2n]));
ax.YTick = get_ytick_range(min_time, max_time);
remove_m_ticks();
xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])

legend("global\_var", "dist\_object", "Location","southeast");
xlabel('Iteraciones');
ylabel('Tiempo (s)');
if (~do_print)
    title('Mean time per size on 2 processes (1 node)');
end
grid on;

if (do_print)
    print("remote_completion_1N_2n", "-dpng");
end

%% 4 processes

sizes_figure_4n = figure("Position", figure_position);

plot(unique_sizes_bytes, data_mean.global_var_2N_4n, "--o")
hold on
plot(unique_sizes_bytes, data_mean.dist_object_2N_4n, "--x")
set(gca, "XScale", "log")
set(gca, "YScale", "log")
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
min_time = min(min([data_mean.global_var_2N_4n, data_mean.dist_object_2N_4n]));
max_time = max(max([data_mean.global_var_2N_4n, data_mean.dist_object_2N_4n]));
ax.YTick = get_ytick_range(min_time, max_time);
remove_m_ticks();

xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])
legend("global\_var", "dist\_object", "Location","southeast");
xlabel('Iteraciones');
ylabel('Tiempo (s)');
if (~do_print)
    title('Mean time per size on 4 processes (2 nodes)');
end
grid on;

if (do_print)
    print("remote_completion_2N_4n", "-dpng");
end

difference_2N = abs(data_mean.global_var_2N_4n ./ data_mean.dist_object_2N_4n);
dispmaxdiff('[2N, global_var]', difference_2N, size_tick_labels);

difference_2N_upcxx = abs(data_mean.dist_object_2N_4n ./ data_mean.global_var_2N_4n);
dispmaxdiff('[2N, dist_object]', difference_2N_upcxx, size_tick_labels);

%% 8 processes

sizes_figure_8n = figure("Position", figure_position);

plot(unique_sizes_bytes, data_mean.global_var_4N_8n, "--o")
hold on
plot(unique_sizes_bytes, data_mean.dist_object_4N_8n, "--x")
set(gca, "XScale", "log")
set(gca, "YScale", "log")
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
min_time = min(min([data_mean.global_var_4N_8n, data_mean.dist_object_4N_8n]));
max_time = max(max([data_mean.global_var_4N_8n, data_mean.dist_object_4N_8n]));
ax.YTick = get_ytick_range(min_time, max_time);
remove_m_ticks();

xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])
legend("global\_var", "dist\_object", "Location","southeast");
xlabel('Iteraciones');
ylabel('Tiempo (s)');
if (~do_print)
    title('Mean time per size on 8 processes (4 nodes)');
end
grid on;

if (do_print)
    print("remote_completion_4N_8n", "-dpng");
end

difference_4N = abs(data_mean.global_var_4N_8n ./ data_mean.dist_object_4N_8n);
dispmaxdiff('[4N, global\_var]', difference_4N, size_tick_labels);

%% 64B: all processes

% Extract time for 64 bytes
size_64_idx = unique_sizes_bytes == 64;

% Initialize arrays for MPI and UPCXX
global_var_time_64B = zeros(length(num_processes), 1);
dist_object_time_64B = zeros(length(num_processes), 1);

% Extract data for each process count
for i = 1:length(num_processes)
    global_var_field = ['global_var_' num2str(num_processes(i)/2) 'N_' num2str(num_processes(i)) 'n'];
    dist_object_field = ['dist_object_' num2str(num_processes(i)/2) 'N_' num2str(num_processes(i)) 'n'];
    
    global_var_time_64B(i) = data_mean.(global_var_field)(size_64_idx);
    dist_object_time_64B(i) = data_mean.(dist_object_field)(size_64_idx);
end

%% Plot time for 64 bytes against number of processes

figure('Position', figure_position);

plot(num_processes, global_var_time_64B, '--o', 'DisplayName', 'global_var')
hold on
plot(num_processes, dist_object_time_64B, '--x', 'DisplayName', 'upcxx')
set(gca, 'YScale', 'log')

remove_m_ticks();
xlim([min(num_processes) max(num_processes)]);
legend("global\_var", "dist\_object", 'Location', 'southeast');
xlabel('Número de procesos');
ylabel('Tiempo (s)');

ax = gca;
ax.XTick = num_processes;
ax.XTickLabel = string(num_processes);

if (~do_print)
    title('Bandwidth for 64 iterations against number of processes');
end

grid on;

if do_print
    print("remote_completion_64I_all", "-dpng");
end

%% Calculate differences for 64B: all processes

difference_64B = abs(global_var_time_64B ./ dist_object_time_64B);
dispmaxdiff('[64B, global_var]', difference_64B, num_processes);

difference_64B_upcxx = abs(dist_object_time_64B ./ global_var_time_64B);
dispmaxdiff('[64B, dist_object]', difference_64B_upcxx, num_processes);

%% Extract time for 4 K iterations
size_16KB_idx = unique_sizes_bytes == 4 * 1024;

% Initialize arrays for MPI and UPCXX
global_var_time_16KB = zeros(length(num_processes), 1);
dist_object_time_16KB = zeros(length(num_processes), 1);

% Extract data for each process count
for i = 1:length(num_processes)
    global_var_field = ['global_var_' num2str(num_processes(i)/2) 'N_' num2str(num_processes(i)) 'n'];
    dist_object_field = ['dist_object_' num2str(num_processes(i)/2) 'N_' num2str(num_processes(i)) 'n'];
    
    global_var_time_16KB(i) = data_mean.(global_var_field)(size_16KB_idx);
    dist_object_time_16KB(i) = data_mean.(dist_object_field)(size_16KB_idx);
end

%% Plot time for 16 KB against number of processes
figure('Position', figure_position);

plot(num_processes, global_var_time_16KB, '--o', 'DisplayName', 'global_var')
hold on
plot(num_processes, dist_object_time_16KB, '--x', 'DisplayName', 'upcxx')
set(gca, 'YScale', 'log')
remove_m_ticks();
xlim([min(num_processes) max(num_processes)])
legend("global\_var", "dist\_object", 'Location', 'southeast');
xlabel('Número de procesos');
ylabel('Tiempo (s)');

ax = gca;
ax.XTick = num_processes;
ax.XTickLabel = num_processes;

if (~do_print)
    title('Time for 16 KB against number of processes');
end
grid on;

if (do_print)
    print("remote_completion_4KI_all", "-dpng");
end

difference_16KB = abs(global_var_time_16KB ./ dist_object_time_16KB);
dispmaxdiff('[16KB, global_var]', difference_16KB, num_processes)

difference_dist_object_16KB = abs(dist_object_time_16KB ./ global_var_time_16KB);
dispmaxdiff('[16KB, dist_object]', difference_dist_object_16KB, num_processes);

