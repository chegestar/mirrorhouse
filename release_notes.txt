MirrorHouse
===========

This Qt Quick example application demonstrates how to use the QCamera from the
Qt Mobility Multimedia module and how to create a circus-style mirror effect 
with it. The application shows how to manipulate camera frames in real time.

This example application is hosted in Nokia Developer Projects:
- http://projects.developer.nokia.com/mirrorhouse

For more information on implementation visit the wiki pages:
- http://projects.developer.nokia.com/mirrorhouse/wiki

What's new in version 1.3
-------------------------
Experimental Harmattan version is merged with the main project.


1. Usage
-------------------------------------------------------------------------------

Touch the mirror to enable get use of secondary camera and try to pinch 
in/out mirror for zooming it.


2. Prerequisites
-------------------------------------------------------------------------------

 - Qt basics
 - Qt Quick basics
 - Manipulating image data manually


3. Project structure and implementation
-------------------------------------------------------------------------------

3.1 Folders
-----------

 |                  The root folder contains the project file, resource files,
 |                  the license information, this file (release notes) and the
 |                  source/qml files.
 |
 |- gfx             Contains application graphics.
 |
 |- icons           Contains application icons.
 |
 |- qtc_packaging   Contains the Harmattan (Debian) packaging files.
 

4. Compatibility
-------------------------------------------------------------------------------

 - Symbian devices with Qt 4.7.4 and Qt Mobility 1.2.1.
 - Nokia N9 (MeeGo 1.2 Harmattan), only experimental target with severe
   performance problems.

Tested to work on Nokia N8-00, E7-00 and N9. Developed with Qt SDK 1.2.

4.1 Required Capabilities
-------------------------

None; The application can be self signed on Symbian.


4.2 Known Issues
----------------

The performance in Harmattan is poor due to the use of Raster graphics system.


5. Building, installing, and running the application
-------------------------------------------------------------------------------

5.1 Preparations
----------------

Check that you have the latest Qt SDK installed in the development environment
and the latest Qt version on the device.

5.2 Using the Qt SDK
--------------------

You can install and run the application on the device by using the Qt SDK.
Open the project in the SDK, set up the correct target (depending on the device
platform), and click the Run button. For more details about this approach,
visit the Qt Getting Started section at Nokia Developer
(http://www.developer.nokia.com/Develop/Qt/Getting_started/).

5.3 Symbian device
------------------

Make sure your device is connected to your computer. Locate the .sis
installation file and open it with Ovi Suite. Accept all requests from Ovi
Suite and the device. Note that you can also install the application by copying
the installation file onto your device and opening it with the Symbian File
Manager application.

After the application is installed, locate the application icon from the
application menu and launch the application by tapping the icon.

5.4 Nokia N9 and Nokia N950
---------------------------

Copy the application Debian package onto the device. Locate the file with the
device and run it; this will install the application. Note that you can also
use the terminal application and install the application by typing the command
'dpkg -i <package name>.deb' on the command line. To install the application
using the terminal application, make sure you have the right privileges 
to do so (e.g. root access).

Once the application is installed, locate the application icon from the
application menu and launch the application by tapping the icon.


6. License
-------------------------------------------------------------------------------

See the license text file delivered with this project. The license file is also
available online at
http://projects.developer.nokia.com/mirrorhouse/browser/trunk/Licence.txt


7. Related documentation
-------------------------------------------------------------------------------
Harmattan camera issues:
 - http://harmattan-dev.nokia.com/docs/library/html/guide/html/Developer_Library_Developing_for_Harmattan_Using_the_device_camera.html

GOOM (Graphics memory handling) 
 - http://wiki.forum.nokia.com/index.php/Graphics_memory_handling


8. Version history
-------------------------------------------------------------------------------

1.3.1 Minor fixes.
1.3.0 Harmattan supporta addend. Memory usage optimizations.
1.2.0 Initial release with the new template.

