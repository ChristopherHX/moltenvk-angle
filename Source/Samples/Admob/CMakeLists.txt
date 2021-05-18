# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#


# Define target name
set (TARGET_NAME Admob)

if(APPLE)

# This little macro lets you set any XCode specific property
macro (set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
    set_property (TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY} ${XCODE_VALUE})
endmacro (set_xcode_property)

if (IOS)
file (GLOB EXTRA_ADMOB_PLUGIN_MM_FILES AdmobPlugin/*.mm)
set (INCLUDE_DIRS ${INCLUDE_DIRS} ../../ThirdParty/GoogleMobileAdsSdkiOS/GoogleMobileAds.xcframework/ios-arm64_armv7/GoogleMobileAds.framework/GMA)
list (APPEND LIBS "-framework GoogleMobileAds")
endif()
endif()

file (GLOB ADMOB_PLUGIN_FILES AdmobPlugin/*.cpp)

# Define source files
define_source_files (EXTRA_H_FILES ${COMMON_SAMPLE_H_FILES} ${ADMOB_PLUGIN_FILES} ${EXTRA_ADMOB_PLUGIN_MM_FILES})

# Setup target with resource copying
setup_main_executable ()

if (IOS)
set_xcode_property(${TARGET_NAME} FRAMEWORK_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/../../ThirdParty/GoogleMobileAdsSdkiOS/GoogleMobileAds.xcframework/ios-arm64_armv7")
endif()

# Setup test cases
setup_test ()
