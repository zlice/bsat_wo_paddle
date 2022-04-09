#include <stdio.h>


int main() {

int a = 1;
int b = 0;
int c = 1;
int d = 0;

//if (a && b || !b && !c && !a) {
//if (a && b || !b) { // true
//if (a && b || !b && c) { // true
//if (a && b || !b && !c) { // false
// () implies || to edge
//
//if (!a && c && b || !b && !a || c && a) { //true
//if (a && b || (!b && c) && (!b && a) ) { // true
//if (!a && !b || !a && a && c && (b || !a && d) ) { // false
//if (a && !b || !a && a && c && (b || !a && d) ) { // true
//if (!a && !b || !a && a && c && (b || !a && d) || a ) { // true
//if (a && !b || !a && a && c && (b || !a && d) ) { // true
//if (a && !b && (b || !c) | ((b | a ) && c )) { // true
//if (a && b && (b || !c) | ((b | a ) && c )) { // false
//if (a && b && (b || !c) | !b) { // false
//if ( (!a && b && (b || !c) ) | ((b | a ) && c )) { // true
//if (a && !b && (b || !c || b | a ) && c ) { // true

//if (a && b && (b || !c) || (b | a) && c ) { // true
//if (a && b && (b || !c) && (b | a) && c ) { // false
//if (a && !b && (b || !c) && (b | a) && c ) { // false
if (a && !b && (!b || !c) && (b | a) && c ) { // true

printf("wtf\n");

}

return 0;

}
