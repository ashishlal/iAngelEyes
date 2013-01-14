#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <string>
#import <iostream>
#import "Interfaces.hpp"
#include <OpenGLES/ES1/gl.h>
#import "PVRTexture.h"
#import "PVRTTexture.h"
//#include <OpenGLES/ES1/glu.h>
#include <OpenGLES/ES1/glext.h>
#include "glpng.h"

using namespace std;

namespace Darwin {
	
class ResourceManager : public IResourceManager {
public:

    string GetResourcePath() const
    {
        NSString* bundlePath =[[NSBundle mainBundle] resourcePath];
        return [bundlePath UTF8String];
    }
#if 0
    TextureDescription LoadPngImage(const string& name)
    {

		NSString* basePath = [NSString stringWithUTF8String:name.c_str()];
		NSString* resourcePath = [[NSBundle mainBundle] resourcePath];
		NSString* fullPath = [resourcePath stringByAppendingPathComponent:basePath];
		
		//NSLog(@"Loading PNG image %s...", fullPath);
		
		UIImage* uiImage = [UIImage imageWithContentsOfFile:fullPath];
		
		//NSString* basePath = [[NSString alloc] initWithUTF8String:name.c_str()];
		
		//NSString *path = [[NSBundle mainBundle] pathForResource:basePath ofType:@"png"];
		//NSData *texData = [[NSData alloc] initWithContentsOfFile:path];
		
		//UIImage *uiImage = [[UIImage alloc] initWithData:texData];
		//if (uiImage == nil) return;

		
		cgImage = uiImage.CGImage;
		m_imageData = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));		
		TextureDescription description;
		description.Size.x = CGImageGetWidth(cgImage);
		description.Size.y = CGImageGetHeight(cgImage);
		bool hasAlpha = CGImageGetAlphaInfo(cgImage) != kCGImageAlphaNone;
		CGColorSpaceRef colorSpace = CGImageGetColorSpace(cgImage);
		switch (CGColorSpaceGetModel(colorSpace)) {
			case kCGColorSpaceModelMonochrome:
				description.Format = 
				hasAlpha ? TextureFormatGrayAlpha : TextureFormatGray;
				break;
			case kCGColorSpaceModelRGB:
				description.Format = 
				hasAlpha ? TextureFormatRgba : TextureFormatRgb;
				break;
			default:
				assert(!"Unsupported color space.");
				break;
		}
		description.BitsPerComponent = CGImageGetBitsPerComponent(cgImage);
		
        //[uiImage release];
        //[basePath release];
		return description;
    }
	void LoadPpngImage(const string& name, void **cgimage, void **data, unsigned int *component, unsigned int *format, int *w, int *h )
    {
#if 1
		//pngRawInfo info;
		
		NSString* basePath = [[NSString alloc] initWithUTF8String:name.c_str()];
       // NSBundle* mainBundle = [NSBundle mainBundle];
       // NSString* fullPath = [mainBundle pathForResource:basePath ofType:@"ppng"];
		NSString* fullPath = [NSTemporaryDirectory() stringByAppendingPathComponent:basePath];
		//UIImage* uiImage = [[UIImage alloc] initWithContentsOfFile:fullPath];
		UIImage* uiImage = [[UIImage alloc] initWithContentsOfFile:fullPath];
		if (uiImage == nil) return;
		*cgimage = uiImage.CGImage;
		
		//const char *fName = [fullPath UTF8String];
		//cout << "fName = " << fullPath << endl;
		//if (!pngLoadRaw(fName, &info)) {
		//	puts("Can't load file");
		//	return;
		//}
		//*data = info.Data;
		//*component = info.Components;
		//*w = info.Width;
		//*h = info.Height;
		//if(*component == 3) *format = GL_RGB;
		//else if(*component == 4) *format = GL_RGBA;
#endif
	}
	void LoadJpgImage(const string& name)
    {
		NSString* basePath = [[NSString alloc] initWithUTF8String:name.c_str()];
      
		NSString *path = [[NSBundle mainBundle] pathForResource:basePath ofType:@"jpg"];
		NSData *texData = [[NSData alloc] initWithContentsOfFile:path];
		
		UIImage *uiImage = [[UIImage alloc] initWithData:texData];
		if (uiImage == nil) return;
		
        //CGImageRef cgImage = uiImage.CGImage;
        //m_imageSize.x = CGImageGetWidth(cgImage);
        //m_imageSize.y = CGImageGetHeight(cgImage);
        //m_imageData = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
#if 1	
		unsigned int width = CGImageGetWidth(uiImage.CGImage);
		unsigned int height = CGImageGetHeight(uiImage.CGImage);
		cgImage = uiImage.CGImage;
		m_imageSize.x = width;
        m_imageSize.y = height;
#if 0
		CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
		imageData = malloc( height * width * 4 );
		CGContextRef context = CGBitmapContextCreate( imageData, width, height, 8, 4 * width, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big );
		CGColorSpaceRelease( colorSpace );
		CGContextClearRect( context, CGRectMake( 0, 0, width, height ) );
		CGContextTranslateCTM( context, 0, height - height );
		CGContextDrawImage( context, CGRectMake( 0, 0, width, height ), uiImage.CGImage );
		size_t bpp = CGImageGetBitsPerPixel(uiImage.CGImage);
		GLenum internal, format;
		CGBitmapInfo info;
		 info       = CGImageGetBitmapInfo(uiImage.CGImage); 
		switch(bpp)
		{
			default:
                assert(0 && "Unknown CGImage bpp");
			case 32:
			{
                internal = GL_RGBA;
                switch(info & kCGBitmapAlphaInfoMask)
                {
					case kCGImageAlphaPremultipliedFirst:
					case kCGImageAlphaFirst:
					case kCGImageAlphaNoneSkipFirst:
						format = GL_BGRA;
						break;
					default:
						format = GL_RGBA;
                }
                break;
			}
			case 24:
                internal = format = GL_RGB;
                break;
			case 16:
                internal = format = GL_LUMINANCE_ALPHA;
                break;
			case 8:
                internal = format = GL_LUMINANCE;
                break;
		}
		
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
		//glTexImage2D(GL_TEXTURE_2D, 0, internal, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
		GLuint errorcode = glGetError();
		if(errorcode != GL_FRAMEBUFFER_COMPLETE_OES) {
		
			//NSLog(@"err = %d\n", errorcode);
		
		}
		CGContextRelease(context);
		[uiImage release];
#endif
        
        [basePath release];
#endif
    }
	
    void* GetImageData()
    {
        return (void*) CFDataGetBytePtr(m_imageData);
		//return imageData;
    }
	void* GetJpgImageData(void **imgData)
    {
		*imgData = imageData;
        return  imageData;
    }
	void GetCGIImage(void **cgi)
	{
		*cgi = (void *)cgImage;
	}
    ivec2 GetImageSize()
    {
        return m_imageSize;
    }
    void UnloadImage()
    {
        CFRelease(m_imageData);
    }
	void UnloadJpgImage()
    {
#ifdef MALLOC_DEBUG
		NTIFree(imageData, (char *)__FILE__, __LINE__);
#else
        free(imageData);
#endif
    }
private:
    CFDataRef m_imageData;
	void *imageData;
	CGImageRef cgImage;
    ivec2 m_imageSize;
#else
	TextureDescription LoadPngImage(const string& file)
	{
		NSString* basePath = [NSString stringWithUTF8String:file.c_str()];
		NSString* resourcePath = [[NSBundle mainBundle] resourcePath];
		NSString* fullPath = [resourcePath stringByAppendingPathComponent:basePath];
		
		//NSLog(@"Loading PNG image %s...", fullPath);
		
		UIImage* uiImage = [UIImage imageWithContentsOfFile:fullPath];
		CGImageRef cgImage = uiImage.CGImage;
		
		//m_imageData = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
		CFDataRef dataRef = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
		m_imageData = [NSData dataWithData:(NSData*) dataRef];
		
		TextureDescription description;
		description.Size.x = CGImageGetWidth(cgImage);
		description.Size.y = CGImageGetHeight(cgImage);
		bool hasAlpha = CGImageGetAlphaInfo(cgImage) != kCGImageAlphaNone;
		CGColorSpaceRef colorSpace = CGImageGetColorSpace(cgImage);
		switch (CGColorSpaceGetModel(colorSpace)) {
			case kCGColorSpaceModelMonochrome:
				description.Format = 
				hasAlpha ? TextureFormatGrayAlpha : TextureFormatGray;
				break;
			case kCGColorSpaceModelRGB:
				description.Format = 
				hasAlpha ? TextureFormatRgba : TextureFormatRgb;
				break;
			default:
				assert(!"Unsupported color space.");
				break;
		}
		description.BitsPerComponent = CGImageGetBitsPerComponent(cgImage);
		
		return description;
	}
	TextureDescription LoadPvrImage(const string& file)
    {
        NSString* basePath = [NSString stringWithUTF8String:file.c_str()];
        NSString* resourcePath = [[NSBundle mainBundle] resourcePath];
        NSString* fullPath = [resourcePath stringByAppendingPathComponent:basePath];
        
        m_imageData = [NSData dataWithContentsOfFile:fullPath];
        m_hasPvrHeader = true;
        PVR_Texture_Header* header = (PVR_Texture_Header*) [m_imageData bytes];
        bool hasAlpha = header->dwAlphaBitMask ? true : false;
		
        TextureDescription description;
        switch (header->dwpfFlags & PVRTEX_PIXELTYPE) {
			case OGL_RGB_565:
				description.Format = TextureFormat565;
				break;
			case OGL_RGBA_5551:
				description.Format = TextureFormat5551;
				break;
			case OGL_RGBA_4444:
				description.Format = TextureFormatRgba;
				description.BitsPerComponent = 4;
				break;
            case OGL_PVRTC2:    
                description.Format = hasAlpha ? TextureFormatPvrtcRgba2 :
				TextureFormatPvrtcRgb2;
                break;
            case OGL_PVRTC4:
                description.Format = hasAlpha ? TextureFormatPvrtcRgba4 :
				TextureFormatPvrtcRgb4;
                break;
            default:
				description.Format = TextureFormatPvrtcRgba4;
                //assert(!"Unsupported PVR image.");
                break;
        }
        
        description.Size.x = header->dwWidth;
        description.Size.y = header->dwHeight;
        description.MipCount = header->dwMipMapCount;
        return description;
    }
    void* GetImageData()
    {
        if (!m_hasPvrHeader)
            return (void*) [m_imageData bytes];
        
        PVR_Texture_Header* header = (PVR_Texture_Header*) [m_imageData bytes];
        char* data = (char*) [m_imageData bytes];
        unsigned int headerSize = header->dwHeaderSize;
        return data + headerSize;
    }
    void UnloadImage()
    {
        m_imageData = 0;
    }
private:
    NSData* m_imageData;
    bool m_hasPvrHeader;
    ivec2 m_imageSize;
#endif
};

IResourceManager* CreateResourceManager()
{
    return new ResourceManager();
}

}