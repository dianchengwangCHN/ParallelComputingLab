#PBS -N PA2-p2
#PBS -l walltime=00:10:00
#PBS -l nodes=1:ppn=28
#PBS -j oe

cd $HOME/PA2-p2

for num in {1..28..1}
do
  ./gcc.out $num > gcc_result_${num}.txt
  ./icc.out $num > icc_result_${num}.txt
done
