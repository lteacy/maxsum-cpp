function genIndexTestData(filename)

file = fopen(filename,'w');

% for each case
for k=1:100

   % generate number of dimensions < 10
   noDims = unidrnd(6);

   % generate size vector 
   siz = unidrnd(15,[1 noDims]);

   % generate sub vector
   sub = ceil(rand(size(siz)) .* siz);

   % generate linear index
   subcell = num2cell(sub);
   ind = sub2ind([siz 1],subcell{:});
   
   % subtract 1 from indices so that they start from 0
   sub = sub-1;
   ind = ind-1;

   % construct line data
   lineData = [noDims ind siz sub];

   % construct line string
   lineStr = [sprintf('%d,',lineData(1:end-1)) num2str(lineData(end))];

   % print line 
   fprintf(file,'%s\n',lineStr);

end

fclose(file);

