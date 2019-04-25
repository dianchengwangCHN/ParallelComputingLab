#PBS -N PA4-p1
#PBS -l walltime=00:10:00
#PBS -l nodes=2:ppn=2
#PBS -j oe


sizes=(1 8 64 512 4096 32768 262144 1048576)
niters=(200000 200000 100000 40000 10000 1000 200 20)

cd $HOME/PA4-p1

mpicc -O3 -o pa4 pa4-pingpong.c
mpicc -O3 -o pa4_nb pa4-pingpong_nb.c

for ((i=0; i<${#sizes[@]}; i++))
do
  size=${sizes[$i]}
  niter=${niters[$i]}
  echo ${size} ${niter} | mpirun -np 2 ./pa4 > result_${size}_wn.txt
  echo ${size} ${niter} | mpirun -np 2 ./pa4_nb > result_${size}_wn_nb.txt
  echo ${size} ${niter} | mpirun -np 2 -ppn 1 ./pa4 > result_${size}_an.txt
  echo ${size} ${niter} | mpirun -np 2 -ppn 1 ./pa4 > result_${size}_an_nb.txt
done
