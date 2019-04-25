#PBS -N PA4-p3
#PBS -l walltime=00:10:00
#PBS -l nodes=2:ppn=28
#PBS -j oe

cd $HOME/PA4-p3

mpicc -O3 -o pa4 pa4-mv.c

for num in 2 4 8 16 32
do
  mpirun -np ${num} ./pa4 > result_${num}.txt
done
