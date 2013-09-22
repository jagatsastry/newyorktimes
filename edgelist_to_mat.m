function edgelist_to_mat()

infile = 'graphs/graphs/all.graph'
outfile = 'graphs/mats/all.mat'

x = load(infile);
entityWordGraph = sparse(x(:, 1), x(:, 2), x(:, 3));
save(outfile, 'entityWordGraph');
outfile
