/*
 *  Copyright 2019 Patrick Stotko
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <iostream>
#include <cmath>

#include <stdgpu/contract.h>        // STDGPU_EXPECTS, STDGPU_ENSURES



float
safe_sqrt(const float x)
{
    STDGPU_EXPECTS(x >= 0.0f);

    float result = std::sqrt(x);

    STDGPU_ENSURES(result >= 0.0f);
    return result;
}


int
main()
{
    std::cout << "In debug mode, a pre-condition failure will be printed right after this line." << std::endl << std::endl;

    float sqrt_m1 = safe_sqrt(-1.0f);

    std::cout << std::endl << "This line is only visible when the contracts are deactivated (release mode) : safe_sqrt(-1.0f) = " << sqrt_m1 << std::endl;
}


