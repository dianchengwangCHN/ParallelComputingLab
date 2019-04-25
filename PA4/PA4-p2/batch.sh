#PBS -N PA4-p2
#PBS -l walltime=00:10:00
#PBS -l nodes=1:ppn=28
#PBS -j oe

cd $HOME/PA4-p2

sizes=(1 8 64 512 4096 32768 262144 1048576)
niters=(1000 1000 500 200 50 10 2 1)

mpicc -O3 -o pa4_nb pa4-ring_nb.c

for ((i=0;i<${#sizes[@]};i++))
do
  size=${sizes[$i]}
  niter=${niters[$i]}
  echo ${size} ${niter} | mpirun -np 24 ./pa4_nb > result_${size}.txt
done 
