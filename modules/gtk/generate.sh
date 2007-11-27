DIR=`pwd`
BUILD_DIR=${DIR}/$1
SRC_DIR=${DIR}/$3
TOP_SRC_DIR=${DIR}/$2

export KREATRIX_MODULES_PATH=${TOP_SRC_DIR}/modules

cd ${SRC_DIR}/generator
${BUILD_DIR}/src/kreatrix generator.kx && 
touch ${DIR}/kxgenerated.tmp
