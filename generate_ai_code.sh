#!/bin/bash
echo "Bash version ${BASH_VERSION}..."



#Check if MODEL_PATH is empty
if [ -z "$1" ]
then
    echo "No argument supplied. Please call '$ generate_ai_code.sh patt/to/your/model/class1_class2.tflite"
    exit 1
fi
#Check if MODEL_PATH has the right format to extract labels
#if [[ $1 == *'.tflite' ]]
#then
#    echo "The model should have a .tflite extension"
#    exit 1
#fi
#
#if [[ $1 == *"_"*".tflite" ]]
#then
#    echo "The model name should have the format class0_class1_classx.tflite"
#    exit 1
#fi


#Get Model path from first parameter
MODEL_PATH=$1

#Generate .c and .h from .tflite with stm32ai
stm32ai generate --name network -m $MODEL_PATH --type tflite --compression none --verbosity 1 --output ./X-CUBE-AI/App --target ./mempools.json --allocate-inputs --series stm32h7 --allocate-outputs --binary --address 0x70000000

#extract labels from network.h
labels=$(grep "AI_NETWORK_ORIGIN_MODEL_NAME" X-CUBE-AI/App/network.h | grep -o '".*"' | grep -oP '(?<=["_]).*?(?=[_"])')

#construct c code for labels
labels_c_array_fixed_part="const char* classes_table[AI_NETWORK_OUT_1_SIZE] = {"
labels_c_array=$labels_c_array_fixed_part
separtor=""
for i in $labels;
do
    labels_c_array+="$separator\"$i\"";
    separator=","
done
labels_c_array+="};"

#dispay for debug
echo $labels_c_array

#replace in app_x-cube-ai.c
escaped_pattern=$(printf '%s\n' "$labels_c_array_fixed_part" | sed -e 's/[]\/$*.^[]/\\&/g');
escaped_replace=$(printf '%s\n' "$labels_c_array" | sed -e 's/[\/&]/\\&/g')
sed -i 's#'"$escaped_pattern"'.*#'"$escaped_replace"'#' Core/Src/main.c


