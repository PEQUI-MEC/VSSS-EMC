################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../mat/impl/dsp/filters/farid_simoncelli.cpp \
../mat/impl/dsp/filters/scharr.cpp \
../mat/impl/dsp/filters/sobel.cpp 

OBJS += \
./mat/impl/dsp/filters/farid_simoncelli.o \
./mat/impl/dsp/filters/scharr.o \
./mat/impl/dsp/filters/sobel.o 

CPP_DEPS += \
./mat/impl/dsp/filters/farid_simoncelli.d \
./mat/impl/dsp/filters/scharr.d \
./mat/impl/dsp/filters/sobel.d 


# Each subdirectory must supply rules for building sources it contributes
mat/impl/dsp/filters/%.o: ../mat/impl/dsp/filters/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


