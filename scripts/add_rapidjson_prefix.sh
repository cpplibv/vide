# Applies renaming within all of the rapidjson source files to add a vide prefix
find ./../include/vide/external/rapidjson/ -type f -name \*.h -exec sed -i "s/RAPIDJSON_/VIDE_RAPIDJSON_/g" {} \;
echo "Remember to backport any vide specific changes not in this version of RapidJSON!"
echo "See https://github.com/USCiLab/cereal/commits/develop/include/cereal/external/rapidjson"
