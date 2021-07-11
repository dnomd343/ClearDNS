SOURCE="$0"
while [ -h "$SOURCE"  ]; do
    DIR="$( cd -P "$( dirname "$SOURCE"  )" && pwd  )"
    SOURCE="$(readlink "$SOURCE")"
    [[ $SOURCE != /*  ]] && SOURCE="$DIR/$SOURCE"
done
DIR="$( cd -P "$( dirname "$SOURCE"  )" && pwd  )"
cd $DIR

let num=1
while read -r cmd
do
  eval $cmd" > data_"$num
  let num++
done < source.sh

files=""
for ((i=1;i<$num;i++))
do
  files="$files data_$i"
done
eval "cat $files | sort -u > data"
eval "rm -f $files"

cat /dev/null > output.txt
while read -r domain
do
flag=$(echo $domain | gawk '/^[a-zA-Z0-9][-a-zA-Z0-9]{0,62}(\.[a-zA-Z0-9][-a-zA-Z0-9]{0,62})+(:[0-9]{1,5})?$/{print $0}')
if [ -n "${flag}" ]; then
  echo "$domain" >> output.txt
fi
done < data
rm data

mv -f output.txt ../gfwlist.txt
