#include <stdlib.h>
#include <string.h>

char *TS_CanonicalizePathName(const char * const in){
    
    unsigned current_stack_cap = 16, current_stack_len = 1;
    unsigned *len_stack = malloc(current_stack_cap*sizeof(unsigned));
    const char **str_stack = malloc(current_stack_cap*sizeof(const char *)),
        *at = in;
    
    int ends_in_slash = 0;
    
    len_stack[0] = 0;
    str_stack[0] = at;
    
    //Calculate all the components.
    while(*at!='\0'){
        switch(*at){
#ifdef _WIN32
            case '\\':
#endif
            case '/':
            /* Allow rooting */
            if(at==in)
                break;
            /* If the current length is 0, as in we just entered this path component,
                and we immediately hit a path seperator, then we just ignore it.
                Increment the current string pointer by one to step over the current character.
            */
            if(len_stack[current_stack_len-1]==0)
                str_stack[current_stack_len-1]++;
            else{ /* Handle an end of path component */
                if(len_stack[current_stack_len-1]==1){
                    if(str_stack[current_stack_len-1][0]=='.'){
                        /* Do not advance the current string size, so that we swallow the '.' and overwrite the current component with the next one. */
                        str_stack[current_stack_len-1] = at+1;
                        len_stack[current_stack_len-1] = 0;
                        break;
                    }
                }
                else if(len_stack[current_stack_len-1]==2){
                    if((str_stack[current_stack_len-1][0]=='.') && (str_stack[current_stack_len-1][1]=='.')){
                        /* Unless this is the first path component, set to overwrite the previous component with the next one. */
                        if(current_stack_len>1){
                            current_stack_len--;
                            str_stack[current_stack_len-1] = at+1;
                            len_stack[current_stack_len-1] = 0;
                            break;
                        }
                    }
                }
                
                if(at[1]=='\0'){
                    ends_in_slash = 1;
                    break;
                }
                current_stack_len++;
                if(current_stack_len>current_stack_cap){
                    current_stack_cap<<=1;
                    len_stack = realloc(len_stack, current_stack_cap*sizeof(unsigned));
                    str_stack = realloc(str_stack, current_stack_cap*sizeof(const char *));
                }
                str_stack[current_stack_len-1] = at+1;
                len_stack[current_stack_len-1] = 0;
            }
            break;
            default:
            len_stack[current_stack_len-1]++;
        }

        at++;
    }
    
    /* Construct the new string */
    {
        char *new_string;
        unsigned i, total_size = 0;
        /* Accumulate total size */
        for(i=0; i<current_stack_len; i++)
            total_size+=len_stack[i];
        
        total_size+=current_stack_len-1;
        
        new_string = malloc(total_size+1);
        new_string[total_size] = 0;
 
        total_size = 0;
        
        for(i=0; i<current_stack_len; i++){
            memcpy(new_string+total_size, str_stack[i], len_stack[i]);
            total_size+=len_stack[i];
            new_string[total_size] = '/';
            total_size++;
        }
        
        if(!ends_in_slash)
            new_string[total_size-1] = '\0';
        
        return new_string;
    }
    
}
