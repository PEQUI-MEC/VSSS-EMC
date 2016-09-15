################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../capture/v4lcap.cpp 

OBJS += \
./capture/v4lcap.o 

CPP_DEPS += \
./capture/v4lcap.d 


# Each subdirectory must supply rules for building sources it contributes
capture/%.o: ../capture/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


