#!bash
#MS2020 Quick script to compile all the zones
#

echo "Debugging dmc/dil compiler"
xxd ~/temp/diku/src1997/zon/midgaard.data > mhex97
xxd ./zon/midgaard.data > mhex20
kompare mhex97 mhex20 

#for u in $(`ls zon/*.zon`)
#do
#  ./DMC/dmc -v $u
#done
