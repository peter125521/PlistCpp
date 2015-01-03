#include "Plist.hpp"
#include <UnitTest++/UnitTest++.h>
#include <iostream>
#include <fstream>
#include <iterator>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

using namespace std;

int height = 256, width = 256;
int vc = 8, hc = 8, anim_flag;
char outfile[256] = "";
char infile[256] = "";
char *frame_prefix = NULL;
char *infile_suffix = NULL;

double delay = 0.2f;
int frames_count_per_anim = 8;

static void createCocoa2xAnimation()
{
    map<string, boost::any> dict;
    cout << "createCocoa2xAnimation start" << endl;

    char anim_name[256] = {0, };
    char frame_name[256] = {0, };

    frames_count_per_anim = hc;

    /* create animations. */
    {
        int id, id_max = vc * hc / frames_count_per_anim;
        int i;
        map<string, boost::any> anim_dict;
        for (id=1; id <= id_max; id++)
        {
            snprintf(anim_name, sizeof(anim_name), "%s_%02d", frame_prefix, id);
            vector<boost::any> array(frames_count_per_anim);
            for (i=0; i < frames_count_per_anim; i++)
            {
                if (infile_suffix)
                    snprintf(frame_name, sizeof(frame_name), "%s_%02d.%s", frame_prefix, 
                    (id-1)*frames_count_per_anim + i + 1, infile_suffix);
                else
                    snprintf(frame_name, sizeof(frame_name), "%s_%02d", frame_prefix, 
                    (id-1)*frames_count_per_anim + i + 1);
                array[i] = string(frame_name);
            }
            map<string, boost::any> anim;
            anim["delay"] = delay;
            anim["frames"] = array;
            anim_dict[anim_name] = anim;
            //anim.clear();
        }
        dict["animations"] = anim_dict;
    }

    cout<<"ready to write plist"<<endl;
    Plist::writePlistXML(outfile, dict);
    dict.clear();

    cout<<"createCocoa2xAnimation finished"<<endl;
}
static void createCocoa2xTexture()
{
    map<string, boost::any> dict;
    cout << "createCocoa2xTexture start" << endl;

    /* create texture. */
    {
        map<string, boost::any> innerDict;
        innerDict["width"] = int(width);
        innerDict["height"] = int(height);
        dict["texture"] = innerDict;
    }

    int v = 0, h = 0;
    int id = 1;
    char buf[256] = {0, };

    /* create frames. */
    {
        map<string, boost::any> frame_dict;

        for (v=0; v<vc; v++)
        {
            for(h=0; h<hc; h++)
            {
                if (infile_suffix)
                    snprintf(buf, sizeof(buf), "%s_%02d.%s", frame_prefix, id, infile_suffix);
                else
                    snprintf(buf, sizeof(buf), "%s_%02d", frame_prefix, id);
                map<string, boost::any> frame;
                frame["x"] = int(h*width/hc);
                frame["y"] = int(v*height/vc);
                frame["width"] = int(width/hc);
                frame["height"] = int(height/vc);
                frame["offsetX"] = int(0);
                frame["offsetY"] = int(0);
                frame["originalWidth"] = int(width/hc);
                frame["originalHeight"] = int(height/vc);
                frame_dict[buf] = frame;
                frame.clear();
                id++;
            }
        }
        dict["frames"] = frame_dict;
    }

    Plist::writePlistXML(outfile, dict);
    dict.clear();

    cout<<"createCocoa2xTexture finished"<<endl;
}

void usage(void)
{
    cerr << "usage: cocoa2dPlist [-a] -i <input file> -W <width> -H <height> -v <vertical div> \
-h <horiz div> -f <output plist file>" << endl;
    exit(1);
}

int main(int argc, char *argv[])
{
    int ch;

    anim_flag = 0;
    while ((ch = getopt(argc, argv, "ai:o:W:H:v:h:")) != -1) {
        switch (ch) {
            case 'v':
                vc = atoi(optarg);
                break;
            case 'h':
                hc = atoi(optarg);
                break;
            case 'W':
                width = atoi(optarg);
                break;
            case 'H':
                height = atoi(optarg);
                break;
            case 'a':
                anim_flag = 1;
                break;
            case 'i':
                strcpy(infile, optarg);
                break;
            case 'o':
                strcpy(outfile, optarg);
                break;
            case '?':
            default:
                usage();
        }
    }

    cout << "animate flag is:" << anim_flag << ", creating "<< 
        ((anim_flag) ? "animation":"texture") << " plist" << endl;;

    if (infile[0])
    {
        if (access(infile, F_OK) != 0)
        {
            cout << "Warning: input file seems not exist." << endl;
        }
        if (outfile[0])
        {
            cout << "input file is specified, your output file will be ignored." << endl;
        }
        char *p = strrchr(infile, '.');
        if(p && *p)
        {
            *p = '\0';
            infile_suffix = p+1;
        }

        frame_prefix = (char *)infile;

        if (!anim_flag)
            sprintf(outfile, "%s.plist", frame_prefix);
        else
            sprintf(outfile, "%s_anim.plist", frame_prefix);

        p = strrchr(infile, '/');

        if(p && *p)
            frame_prefix = p + 1;
    }

    cout << "output file is:" << outfile << endl;
    cout << "input file is:" << infile << endl;
    cout << "width is:" << width << endl;
    cout << "height is:" << height << endl;
    cout << "vc is:" << vc << endl;
    cout << "hc is:" << hc << endl;

    if (!infile[0] || !outfile[0])
    {
        cerr << "please specify input file." << endl;
        usage();
    }

    if (!anim_flag)
    {
        createCocoa2xTexture();
    }
    else
    {
        createCocoa2xAnimation();
    }


    return 0;
}
