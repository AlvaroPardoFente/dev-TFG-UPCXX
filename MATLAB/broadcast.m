%% Import and process data

mpi_2n = importtable("mpi_broadcast_array_2n.csv");
upcxx_2n = importtable("upcxx_broadcast_array_2n.csv");

mpi_4n = importtable("mpi_broadcast_array_4n.csv");
upcxx_4n = importtable("upcxx_broadcast_array_4n.csv");

mpi_8n = importtable("mpi_broadcast_array_8n.csv");
upcxx_8n = importtable("upcxx_broadcast_array_8n.csv");

mpi_12n = importtable("mpi_broadcast_array_12n.csv");
upcxx_12n = importtable("upcxx_broadcast_array_12n.csv");

unique_sizes = unique(mpi_8n.Size);
unique_sizes_categorical = categorical(unique_sizes);

mpi_2n_clean_data = rmopersize(mpi_2n);
upcxx_2n_clean_data = rmopersize(upcxx_2n);

mpi_4n_clean_data = rmopersize(mpi_4n);
upcxx_4n_clean_data = rmopersize(upcxx_4n);

mpi_8n_clean_data = rmopersize(mpi_8n);
upcxx_8n_clean_data = rmopersize(upcxx_8n);

mpi_12n_clean_data = rmopersize(mpi_12n);
upcxx_12n_clean_data = rmopersize(upcxx_12n);

%% 8 nodes: Times in different sizes

mpi_bandwidth = mpi_8n_clean_data.Size ./ mpi_8n_clean_data.Time;
mpi_mean_bandwidth = arrayfun(@(size) mean(mpi_bandwidth(mpi_8n_clean_data.Size == size)), unique_sizes);
upcxx_bandwidth = upcxx_8n_clean_data.Size ./ upcxx_8n_clean_data.Time;
upcxx_mean_bandwidth = arrayfun(@(size) mean(upcxx_bandwidth(upcxx_8n_clean_data.Size == size)), unique_sizes);

mpi_mean_times = arrayfun(@(size) mean(mpi_8n_clean_data.Time(mpi_8n_clean_data.Size == size)), unique_sizes);
upcxx_mean_times = arrayfun(@(size) mean(upcxx_8n_clean_data.Time(upcxx_8n_clean_data.Size == size)), unique_sizes);

figure

tiledlayout("flow");

% Se eliminan outliers para cada tamaño y se hace la media
nexttile
loglog(unique_sizes, mpi_mean_times, "-o", "DisplayName", "mpi mean")
hold on
loglog(unique_sizes, upcxx_mean_times, "-o", "DisplayName", "upcxx mean")

legend("mpi", "upcxx");
xlabel('Size');
ylabel('Time (s)');
title('Mean times without outliers');
grid on;

nexttile
plot(unique_sizes, mpi_mean_bandwidth, "--o")
hold on
plot(unique_sizes, upcxx_mean_bandwidth, "--o")
set(gca, "XScale", "log")
legend("mpi", "upcxx");
xlabel('Size');
ylabel('Bandwidth(4B/s)');
title('Mean bandwidth per size');
grid on;

nexttile
bar(unique_sizes_categorical, [mpi_mean_bandwidth, upcxx_mean_bandwidth])
legend("mpi", "upcxx");
xlabel('Size');
ylabel('Bandwidth(4B/s)');
title('Mean bandwidth per size');
grid on;

%% 4MB: Times in different number of nodes

num_nodes = [2, 4, 8, 12];
size = 4 * 1024 * 1024;

mpi_2n_4M = mpi_2n_clean_data(mpi_2n_clean_data.Size == size, :);
mpi_4n_4M = mpi_4n_clean_data(mpi_4n_clean_data.Size == size, :);
mpi_8n_4M = mpi_8n_clean_data(mpi_8n_clean_data.Size == size, :);
mpi_12n_4M = mpi_12n_clean_data(mpi_12n_clean_data.Size == size, :);

upcxx_2n_4M = upcxx_2n_clean_data(upcxx_2n_clean_data.Size == size, :);
upcxx_4n_4M = upcxx_4n_clean_data(upcxx_4n_clean_data.Size == size, :);
upcxx_8n_4M = upcxx_8n_clean_data(upcxx_8n_clean_data.Size == size, :);
upcxx_12n_4M = upcxx_12n_clean_data(upcxx_12n_clean_data.Size == size, :);

mpi_mean_times_4M = [
    mean(mpi_2n_4M.Time);
    mean(mpi_4n_4M.Time);
    mean(mpi_8n_4M.Time);
    mean(mpi_12n_4M.Time)
];

upcxx_mean_times_4M = [
    mean(upcxx_2n_4M.Time);
    mean(upcxx_4n_4M.Time);
    mean(upcxx_8n_4M.Time);
    mean(upcxx_12n_4M.Time)
];

figure
plot(num_nodes, mpi_mean_times_4M, '-o', 'DisplayName', 'MPI Mean Time');
hold on;
plot(num_nodes, upcxx_mean_times_4M, '-o', 'DisplayName', 'UPCXX Mean Time');
xlabel('Number of Nodes');
ylabel('Mean Time (s)');
title('Mean Times for Size = 4M Across Different Node Counts');
legend("mpi", "upcxx");
grid on;