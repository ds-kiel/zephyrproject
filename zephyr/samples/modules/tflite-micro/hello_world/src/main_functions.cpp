/*
 * Copyright 2020 The TensorFlow Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "main_functions.h"

#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include "constants.h"
#include "model.hpp"
#include "output_handler.hpp"
#include <tensorflow/lite/micro/micro_log.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/system_setup.h>
#include <tensorflow/lite/schema/schema_generated.h>

#include <zephyr/kernel.h>
#include <zephyr/timing/timing.h>

// Change input size and tensor arena size based on your model
#define INPUT_SIZE (126 * 95 * 1)
// #define INPUT_SIZE (48 *48 *3)
// #define INPUT_SIZE (64 *64 *3)

/* Globals, used for compatibility with Arduino-style sketches. */
namespace {
	const tflite::Model *model = nullptr;
	tflite::MicroInterpreter *interpreter = nullptr;
	TfLiteTensor *input = nullptr;
	TfLiteTensor *output = nullptr;
	int inference_count = 0;

	constexpr int kTensorArenaSize = 250000;
	uint8_t tensor_arena[kTensorArenaSize];
}  /* namespace */

/* The name of this function is important for Arduino compatibility. */
void setup(void)
{
	/* Map the model into a usable data structure. This doesn't involve any
	 * copying or parsing, it's a very lightweight operation.
	 */
	model = tflite::GetModel(g_model);
	if (model->version() != TFLITE_SCHEMA_VERSION) {
		MicroPrintf("Model provided is schema version %d not equal "
					"to supported version %d.",
					model->version(), TFLITE_SCHEMA_VERSION);
		return;
	}
	
	/* This pulls in the operation implementations we need.
	 * NOLINTNEXTLINE(runtime-global-variables)
	 */
	// MCUNet
	// static tflite::MicroMutableOpResolver<8> resolver;
	// if (resolver.AddQuantize()!= kTfLiteOk){return;}
	// if (resolver.AddDepthwiseConv2D()!= kTfLiteOk) {return;}
	// if (resolver.AddConv2D()!= kTfLiteOk){return;}
	// if (resolver.AddAveragePool2D()!= kTfLiteOk){return;}
	// if (resolver.AddPad()!= kTfLiteOk){return;}
	// if (resolver.AddAdd()!= kTfLiteOk){return;}
	// if (resolver.AddReshape()!= kTfLiteOk){return;}
	// if (resolver.AddDequantize()!= kTfLiteOk){return;}
 
	// MobileNetv3-s tensorflow
	// static tflite::MicroMutableOpResolver<12> resolver;
	// if (resolver.AddQuantize()!= kTfLiteOk){return;}
	// if (resolver.AddDequantize()!= kTfLiteOk){return;}
	// if (resolver.AddDepthwiseConv2D()!= kTfLiteOk) {return;}
	// if (resolver.AddConv2D()!= kTfLiteOk){return;}
	// if (resolver.AddAveragePool2D()!= kTfLiteOk){return;}
	// if (resolver.AddPad()!= kTfLiteOk){return;}
	// if (resolver.AddAdd()!= kTfLiteOk){return;}
	// if (resolver.AddMul()!= kTfLiteOk){return;}
	// if (resolver.AddHardSwish()!= kTfLiteOk){return;}
	// if (resolver.AddReshape()!= kTfLiteOk){return;}
	// if (resolver.AddSoftmax()!= kTfLiteOk){return;}
	// if (resolver.AddMean()!= kTfLiteOk){return;}

	// MobileNetv3-s Pytorch
	static tflite::MicroMutableOpResolver<11> resolver;
	if (resolver.AddQuantize()!= kTfLiteOk){return;}
	if (resolver.AddDequantize()!= kTfLiteOk){return;}
	if (resolver.AddDepthwiseConv2D()!= kTfLiteOk) {return;}
	if (resolver.AddConv2D()!= kTfLiteOk){return;}
	if (resolver.AddPad()!= kTfLiteOk){return;}
	if (resolver.AddAdd()!= kTfLiteOk){return;}
	if (resolver.AddMul()!= kTfLiteOk){return;}
	if (resolver.AddSum()!= kTfLiteOk){return;}
	if (resolver.AddHardSwish()!= kTfLiteOk){return;}
	if (resolver.AddReshape()!= kTfLiteOk){return;}
	if (resolver.AddFullyConnected()!= kTfLiteOk){return;}

	// LSTM
	// static tflite::MicroMutableOpResolver<6> resolver;
	// if (resolver.AddQuantize()!= kTfLiteOk) {return;}
	// if (resolver.AddUnidirectionalSequenceLSTM()!= kTfLiteOk) {return;}
	// if (resolver.AddStridedSlice()!= kTfLiteOk){return;}
	// if (resolver.AddFullyConnected()!= kTfLiteOk){return;}
	// if (resolver.AddSoftmax()!= kTfLiteOk){return;}
	// if (resolver.AddDequantize()!= kTfLiteOk){return;}

	/* Build an interpreter to run the model with. */
	static tflite::MicroInterpreter static_interpreter(
		model, resolver, tensor_arena, kTensorArenaSize);
	interpreter = &static_interpreter;

	/* Allocate memory from the tensor_arena for the model's tensors. */
	TfLiteStatus allocate_status = interpreter->AllocateTensors();
	if (allocate_status != kTfLiteOk) {
		MicroPrintf("AllocateTensors() failed");
		return;
	}

	/* Obtain pointers to the model's input and output tensors. */
	input = interpreter->input(0);
	output = interpreter->output(0);

	/* Keep track of how many inferences we have performed. */
	inference_count = 0;
}

/* The name of this function is important for Arduino compatibility. */
void loop(void)
{

	float * input_buf = (float *)((input->data.data));

	for(size_t i = 0; i < INPUT_SIZE; i++) {
			input_buf[i] = 0.5;
	}

	// Get time for inference latency
	// static timing_t dbts_start_ts = 0;
	// static timing_t dbts_end_ts = 0;	
	// timing_init();
	// timing_start(); 
	// dbts_start_ts = timing_counter_get();
	int64_t start_time = k_uptime_get();
	
	/* Run inference, and report any error */
	TfLiteStatus invoke_status = interpreter->Invoke();
	if (invoke_status != kTfLiteOk) {
		MicroPrintf("Invoke failed on Inference");
		return;
	}

	// dbts_end_ts = timing_counter_get();
	// timing_stop();
	// MicroPrintf("Time exec = %llu\n",timing_cycles_to_ns(timing_cycles_get(&dbts_start_ts, &dbts_end_ts)));
	// MicroPrintf("Required memory... %i bytes.\n", interpreter->arena_used_bytes());	
	int64_t end_time = k_uptime_delta(&start_time);
	MicroPrintf("Time exec = %lld\n",end_time);
	
    inference_count += 1;
	if (inference_count >= kInferencesPerCycle) inference_count = 0;
}
