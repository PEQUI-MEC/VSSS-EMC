################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../mat/impl/util/msg.c 

CPP_SRCS += \
../mat/impl/util/list.cpp \
../mat/impl/util/mem.cpp 

OBJS += \
./mat/impl/util/list.o \
./mat/impl/util/mem.o \
./mat/impl/util/msg.o 

C_DEPS += \
./mat/impl/util/msg.d 

CPP_DEPS += \
./mat/impl/util/list.d \
./mat/impl/util/mem.d 


# Each subdirectory must supply rules for building sources it contributes
mat/impl/util/%.o: ../mat/impl/util/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

mat/impl/util/%.o: ../mat/impl/util/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


