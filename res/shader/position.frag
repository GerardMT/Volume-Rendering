#version 330

out vec3 out_pos_word;

smooth in vec3 pos_world;

void main (void) {
    out_pos_word = pos_world;
}
