 #include <stdio.h>
 
 /* Get all OS and signal processing YARP classes */
 
 #include <yarp/os/all.h>
 #include <yarp/sig/all.h>
 
 using namespace yarp::os;
 using namespace yarp::sig;
 using namespace std; 

int main() {

 Network yarp; // set up yarp

 BufferedPort<ImageOf<PixelRgb> > imagePort_in;  // make a port for reading images
 BufferedPort<ImageOf<PixelRgb> > imagePort_out; // make a port for sending images

 imagePort_in.open("/image/in");  // give the port a name
 imagePort_out.open("/cropped_image/out");

 // ImagePort_in receives input from webcam
 Network::connect("/movie/out","/image/in");
 
 while (1) { // repeat forever
   ImageOf<PixelRgb> *image = imagePort_in.read();  // read an image
   ImageOf<PixelRgb> &image_cropped = imagePort_out.prepare();

   ImageOf<PixelRgb> *image_480= imagePort_in.read();
   image_480->resize(480,480);
   
   if (image!=NULL) { // check we actually got something
      //printf("We got an image of size %dx%d\n", image->width(), image->height()); 
   
   // Crop 640*480 image centrally to 480*480     
   for (int x=0; x<480; x++) {
        for (int y=0; y<480; y++) {
          PixelRgb& pixel_ = image->pixel(x+80,y);          
	  image_480->pixel(x,y)=pixel_;       
        }
      }

    image_cropped.copy(*image_480, 256, 256);
    imagePort_out.write();
   }
 }
 return 0;
}

