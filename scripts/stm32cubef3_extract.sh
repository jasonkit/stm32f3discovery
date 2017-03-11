#!/bin/sh

DefaultLibDestPath=./lib
DefaultTemplateDestPath=./template

STM32CubeF3Path=$1
LibDestPath=${2:-$DefaultLibDestPath}
TemplateDestPath=${3:-$DefaultTemplateDestPath}

function copy_dir {
  for i in ${SourcePaths[@]}
  do
    mkdir -p ${LibDestPath}/`dirname $2/$i`
    cp -R ${STM32CubeF3Path}/$1/$i `dirname ${LibDestPath}/$2/$i`
  done
}

# Handle Lib
mkdir -p ${LibDestPath}

# Extract CMSIS
CMSISPath=Drivers/CMSIS
CMSISDestPath=CMSIS
SourcePaths=(
  DSP_Lib/license.txt
  DSP_Lib/Source
  Device/ST/STM32F3xx/Include/stm32f303xc.h
  Device/ST/STM32F3xx/Include/stm32f3xx.h
  Device/ST/STM32F3xx/Include/system_stm32f3xx.h
  Device/ST/STM32F3xx/Source/Templates/system_stm32f3xx.c
  Device/ST/STM32F3xx/Source/Templates/gcc/startup_stm32f303xc.s
  Device/ST/STM32F3xx/Source/Templates/gcc/linker/STM32F303XC_FLASH.ld
  Include
  Lib/license.txt
  Lib/GCC
  RTOS
  CMSIS_END_USER_LICENCE_AGREEMENT.rtf
)
copy_dir ${CMSISPath} ${CMSISDestPath}

# Extract BSP library
BSPPath=Drivers/BSP
SourcePaths=(
  Components
  STM32F3-Discovery/*.c
  STM32F3-Discovery/*.h
)
copy_dir ${BSPPath} ${BSPPath}

# Extract HAL library
HALPath=Drivers/STM32F3xx_HAL_Driver
HALDestPath=Drivers/HAL
SourcePaths=(
  Inc
  Src
)
copy_dir ${HALPath} ${HALDestPath}

# Extract FreeRTOS
FreeRTOSPath=Middlewares/Third_Party/FreeRTOS
FreeRTOSDestPath=FreeRTOS
SourcePaths=(
  License
  Source/*.c
  Source/*.txt
  Source/CMSIS_RTOS
  Source/include
  Source/portable/readme.txt
  Source/portable/MemMang/*.c
  Source/portable/GCC/ARM_CM4_MPU
  Source/portable/GCC/ARM_CM4F
  Source/portable/Common
)
copy_dir ${FreeRTOSPath} ${FreeRTOSDestPath}

# Extract USB library
SourcePaths=(
 STM32_USB_Device_Library 
)
copy_dir Middlewares/ST . 

find ${LibDestPath} -name *.html -exec rm -f {} \;

# Prepare common dir
mkdir -p ${LibDestPath}/common

# create start-gdb.cmd
FilePath=${LibDestPath}/common/start-gdb.cmd
echo "target remote localhost:3333" >> ${FilePath}
echo "monitor reset" >> ${FilePath}
echo "monitor halt" >> ${FilePath}

# create stm32f3discovery.cfg
FilePath=${LibDestPath}/common/stm32f3discovery.cfg
echo "source [find interface/stlink-v2.cfg]" >> ${FilePath}
echo "source [find target/stm32f3x.cfg]" >> ${FilePath}
echo "reset_config srst_only srst_nogate" >> ${FilePath}

pushd ${LibDestPath}/common > /dev/null

ln -s ../${CMSISDestPath}/Device/ST/STM32F3xx/Source/Templates/gcc/linker/STM32F303XC_FLASH.ld stm32f303vc.ld
ln -s ../${CMSISDestPath}/Device/ST/STM32F3xx/Source/Templates/gcc/startup_stm32f303xc.s startup.s

popd > /dev/null

# Handle Template
mkdir -p ${TemplateDestPath}
mkdir -p ${TemplateDestPath}/include
mkdir -p ${TemplateDestPath}/src

TemplateProjectPath=Projects/STM32F3-Discovery/Applications/FreeRTOS/FreeRTOS_ThreadCreation
cp ${STM32CubeF3Path}/${TemplateProjectPath}/Inc/* ${TemplateDestPath}/include/
cp ${STM32CubeF3Path}/${TemplateProjectPath}/Src/* ${TemplateDestPath}/src/
