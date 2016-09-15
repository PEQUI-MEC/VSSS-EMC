
// =============================================================================
void mem_copy_function(char * src, char * dest, int nbytes) {
	while (nbytes) {
		*dest = *src;
		dest++;
		src++;
		nbytes--;
	}
}
void mem_clear_function(char * dest, int nbytes) {
	while (nbytes) {
		*dest = 0;
		dest++;
		nbytes--;
	}
}
