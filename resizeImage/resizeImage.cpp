 #include <stdio.h>
 
 /* Get all OS and signal processing YARP classes */
 
 #include <yarp/os/all.h>
 #include <yarp/sig/all.h>
 
 using namespace yarp::os;
 using namespace yarp::sig;
 
int main() {

 Network yarp; // set up yarp

 BufferedPort<ImageOf<PixelRgb> > imagePort_in;  // make a port for reading images
 BufferedPort<ImageOf<PixelRgb> > imagePort_out; // make a port for sending images

 imagePort_in.open("/image/in");  // give the port a name
 imagePort_out.open("/resized_image/out");

 // ImagePort_in receives input from webcam
 Network::connect("/movie/out","/image/in");
 
 while (1) { // repeat forever
   ImageOf<PixelRgb> *image = imagePort_in.read();  // read an image
   ImageOf<PixelRgb> &image_cropped = imagePort_out.prepare();

   image_cropped.copy(*image, 256, 256);

   if (image!=NULL) { // check we actually got something
      //printf("We got an image of size %dx%d\n", image->width(), image->height()); 
   
    imagePort_out.write();
   }
 }
 return 0;
}

