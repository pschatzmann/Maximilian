#pragma once
#include "libs/maxiMalloc.h"
#ifdef ARDUINO
# include "Arduino.h"
# include <cstddef> // for size_t
# include <limits>
# include <string.h> // for memset
# define USE_MAXVECTOR
# undef min
# undef max
# undef PI
#endif

typedef float maxi_float_t;


/// A simple audio data store implementation that can hold data from progmem
class maxi_vector {
    public:
        maxi_vector(){
        }
        ~maxi_vector(){
            if (!is_const && p_data!=nullptr){
                free(p_data);
            }
        }

        /// Constructor as template so that we can determine the length of the array
        template <size_t size> 
        maxi_vector(const maxi_float_t (&array)[size]){
            p_data = (maxi_float_t*)array;
            len = size;
            is_const = true;
        }

        void set(const maxi_vector &from){
            p_data = from.p_data;
            len = from.len;
            is_const = from.is_const;
        }

        maxi_float_t operator[](size_t idx) const {
            static maxi_float_t undefined = 0.0f;
            return idx<len?p_data[idx]:undefined;
        }   

        maxi_float_t& operator[](size_t idx){
            static maxi_float_t undefined = 0.0f;
            return idx<len?p_data[idx]:undefined;
        }   

        void clear() {
            if (!is_const){
                memset((void*)p_data,0,len*sizeof(maxi_float_t));
            } else {
                Serial.println("Error: clear not supported");
            }
        } 

        size_t size() {
            return len;
        }

        void resize(size_t len){
            if (!is_const){
                if (p_data!=nullptr) free(p_data);
                p_data = (maxi_float_t*) maxi_malloc(len * sizeof(maxi_float_t)); 
                memset(p_data, 0, len * sizeof(maxi_float_t));
                this->len = len;
            } else {
                Serial.println("Error: resize not supported");
            }
        }

        void normalize(maxi_vector &in){
          //vector<maxi_float_t> outvar = vector<maxi_float_t>(invar.begin(), invar.end()); //emplace into new variable  
          set(in);
          is_const = true;
        }

        maxi_float_t* data(){
            return p_data;
        }

    protected:
        bool is_const=false;
        size_t len=0;
        maxi_float_t *p_data=nullptr;

};