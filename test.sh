declare -a x=(1 4 3 7 10 9 15)
declare -a y=(1 8 3 6 2 9 15)

echo "Running test.sh"
for i in {0..6..1};
do
	echo "Testing x=${x[$i]}, y=${y[$i]}"
	touch ./data/logs/"$i"_x"${x[$i]}"_y"${y[$i]}".log
	./bin/privPointLoc main 6 "${x[$i]}" "${y[$i]}" > ./data/logs/b6-2_"$i"_x"${x[$i]}"_y"${y[$i]}".log
done
