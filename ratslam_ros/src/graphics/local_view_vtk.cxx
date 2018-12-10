#include <vtkSphereSource.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkVersion.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleImage.h>
#include <vtkImageMapper.h>
#include <vtkActor2D.h>
#include <vtkImageSlice.h>
#include <vtkImageLuminance.h>
#include <vtkProperty2D.h>
#include <vtkLight.h>
#include <vtkCallbackCommand.h>

#include <vtkTestFilter.h>

#include <ros/ros.h>

#include "../ratslam/local_view_match.h"

void CreateColorImage(vtkImageData* image,const unsigned char* view);
void CreateColorImage(vtkImageData* image,const double* view);

void CallbackFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData )
{
//  std::cout << "CallbackFunction called." << std::endl;
}
vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
vtkSmartPointer<vtkRenderWindowInteractor>::New();
vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
vtkSmartPointer<vtkImageData> colorImage = vtkSmartPointer<vtkImageData>::New();
vtkSmartPointer<vtkImageMapper> imageMapper = vtkSmartPointer<vtkImageMapper>::New();
vtkSmartPointer<vtkActor2D> imageActor = vtkSmartPointer<vtkActor2D>::New();
vtkSmartPointer<vtkTestFilter> filter = vtkSmartPointer<vtkTestFilter>::New();
vtkSmartPointer<vtkCallbackCommand> callback = vtkSmartPointer<vtkCallbackCommand>::New();

int local_view_vtk_show(const unsigned char* view_rgb,const double* view_temple,const double* view_current)
{
  renderWindow->SetSize(640, 600);
  renderWindow->SetWindowName("openRatSLAM Local View");
  renderWindowInteractor->SetRenderWindow(renderWindow);
  // Define viewport ranges
  double xmins[3] = {0,  0,      0};
  double xmaxs[3] = {1,  1,      1};
  double ymins[3] = {0,   0.1,  .2};
//  double ymins[3] = {0,   0.25,  0};
  double ymaxs[3]=  {0.1,0.2,    1};
  for(unsigned i = 0; i < 3; i++)
    {
    renderWindow->AddRenderer(renderer);
    renderer->SetViewport(xmins[i],ymins[i],xmaxs[i],ymaxs[i]);
    // Create an image
    switch(i)
    {
		case 0:
			CreateColorImage(colorImage, view_current);
			break;
		case 1:
			CreateColorImage(colorImage, view_temple);
			break;
		case 2:
			CreateColorImage(colorImage, view_rgb);
			break;
    }

  #if VTK_MAJOR_VERSION <= 5
    imageMapper->SetInputConnection(colorImage->GetProducerPort());
  #else
    imageMapper->SetInputData(colorImage);
  #endif
    imageMapper->SetColorWindow(255);
    imageMapper->SetColorLevel(127.5);
    imageActor->SetMapper(imageMapper);
//    vtkProperty2D *property2D = imageActor->GetProperty();
//    property2D->SetOpacity(0.1);
    renderer->AddActor(imageActor);
    renderer->ResetCamera();
//    renderer->SetBackground(.4, .5, .6);
    renderWindow->Render();
    }
  renderWindowInteractor->Initialize();
  renderWindowInteractor->SetRenderWindow(renderWindow);

//  renderWindowInteractor->Start();
  return EXIT_SUCCESS;
}
void CreateColorImage(vtkImageData* image,const unsigned char* view)
{
  unsigned int dimx = 640;//image width
  unsigned int dimy = 480;//image hight

  image->SetDimensions(dimx, dimy, 1);

#if VTK_MAJOR_VERSION <= 5
  image->SetNumberOfScalarComponents(3);//three components:rgb
  image->SetScalarTypeToUnsignedChar();
  image->AllocateScalars();
#else
  image->AllocateScalars(VTK_UNSIGNED_CHAR,3);
#endif
  unsigned int image_index = 0;
//  unsigned char* pixel = static_cast<unsigned char*>(image->GetScalarPointer());

	for (unsigned int x = 0; x < dimx; x++) {
		for (unsigned int y = 0; y < dimy; y++) {
			unsigned char* pixel = static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));

			image_index = (x + (dimy - y - 1)*dimx)*3;
			//bgr8 --> rgb8
			pixel[2] = view[image_index];
			pixel[1] = view[image_index + 1];
			pixel[0] = view[image_index + 2];
		}
	}
	image->Modified();
}
void CreateColorImage(vtkImageData* image,const double* view)
{
  unsigned int dimx = 640;//image width
  unsigned int dimy = 60;//image hight

  image->SetDimensions(dimx, dimy, 1);
//  image->SetSpacing(10 , 6, 1.0);

#if VTK_MAJOR_VERSION <= 5
  image->SetNumberOfScalarComponents(1);//three components:rgb
  image->SetScalarTypeToUnsignedChar();
  image->AllocateScalars();
#else
  image->AllocateScalars(VTK_UNSIGNED_CHAR,1);
#endif
  unsigned int image_index = 0;
//  unsigned char* pixel = static_cast<unsigned char*>(image->GetScalarPointer());

	for (unsigned int x = 0; x < dimx; x++) {
		for (unsigned int y = 0; y < dimy; y++) {
			unsigned char* pixel = static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));
//			image_index = (x + (dimy - y - 1)*dimx);
			image_index = (x/10 + (dimy/6 - y/6 - 1)*dimx/10);
//			image_index = (x + (y)*dimx)*3;
			pixel[0] = (unsigned char)(view[image_index]*255);
//			pixel[1] = view[image_index + 1];
//			image_index = (x + (y)*dimx)*3;
//			pixel[2] = view[image_index + 2];
//			ROS_FATAL_STREAM(""<<(int)pixel[0]);
		}
	}
	image->Modified();
}
