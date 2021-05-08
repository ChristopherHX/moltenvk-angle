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
if (IOS)
set (ADMOB_LIBPATH  ../../Urho3D/ThirdParty/GoogleMobileAdsSdkiOS)
set (ADMOB_LIBS
    "-framework ../../Urho3D/ThirdParty/GoogleMobileAdsSdkiOS/GoogleMobileAds"
    
set (LIBS  ${LIBS}  ${ADMOB_LIBS})
link_directories(. ${ADMOB_LIBPATH})
)
file (GLOB EXTRA_ADMOB_PLUGIN_MM_FILES AdmobPlugin/*.mm)
endif()
endif()

file (GLOB ADMOB_PLUGIN_FILES AdmobPlugin/*.cpp)

# Define source files
define_source_files (EXTRA_H_FILES ${COMMON_SAMPLE_H_FILES} ${ADMOB_PLUGIN_FILES} ${EXTRA_ADMOB_PLUGIN_MM_FILES})

# Setup target with resource copying
setup_main_executable ()

# Setup test cases
setup_test ()
