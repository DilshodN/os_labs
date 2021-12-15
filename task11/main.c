#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>


int execvpe(const char* file, char* const argv[], char* const envp[]) {
    size_t num_of_envp = 0;

    for(int i = 0; envp[i]; ++i){
        num_of_envp++;
    }

    size_t size_of_envp_array = num_of_envp * sizeof (*envp);
    char** env_backup = malloc(size_of_envp_array);
    if(env_backup == NULL){
        perror("Allocation Error\n");
        return -1;
    }

    memcpy(env_backup, envp, size_of_envp_array);

    char** new_env_variables = NULL;
    size_t num_of_new_env_variables = 0;
    // backup-ing
    for(int i = 0; i < num_of_envp; i++){
        char *buffer, *env_variable;

        env_variable = strdup(env_backup[i]);
        if(env_variable == NULL){
            free(env_backup);
            perror("Insufficient memory was available\n");
            return -1;
        }

        buffer = strsep(&env_variable, "=");
        if(buffer == NULL){
            free(env_backup);
            perror("Wrong format of env variables\n");
            return -1;
        }

        char* native_env_variable = getenv(buffer);

        if(native_env_variable == NULL){
            num_of_new_env_variables++;
            new_env_variables = realloc(new_env_variables, num_of_new_env_variables * sizeof(*envp));
            if(new_env_variables == NULL){
                perror("Allocation Error\n");
                return -1;
            }
            new_env_variables[num_of_new_env_variables - 1] = buffer;
        }

        strcat(buffer, "=");
        strcat(buffer, native_env_variable);
        env_backup[i] = buffer;
    }


    for (int i = 0; i < num_of_envp; ++i) {
        if (putenv(envp[i]) != 0) {
            for(int j = 0; j < num_of_envp; ++j){
                putenv(env_backup[j]);
            }
            for(int k = 0; k < num_of_new_env_variables; ++k){
                unsetenv(new_env_variables[k]);
            }
            perror("putenv() error occurred\n");
            free(env_backup);
            free(new_env_variables);
            return -1;
        }
    }

    if(execvp(file, argv) == -1) {
        for(int j = 0; j < num_of_envp; ++j){
            putenv(env_backup[j]);
        }
        for(int k = 0; k < num_of_new_env_variables; ++k){
            unsetenv(new_env_variables[k]);
        }
        perror("execvp() error\n");
        free(env_backup);
        free(new_env_variables);
        return -1;
    }

    free(env_backup);
    free(new_env_variables);
    return 0;
}

int main(int argc, char* argv[]) {
    char* args[] = {"~", NULL};
    char* envp[] = {"USER=A", "PWD=B", "HOME=C", NULL};
    if (execvpe("cd", args, envp) == -1) {
        perror("execvpe");
    }
    printf("%s", getenv("USER"));
    return -1;
}
