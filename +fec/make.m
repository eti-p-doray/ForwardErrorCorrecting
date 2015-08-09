function make
% Build all required mex files for MapCode
% This function should be called once before any attempt to use MapCode
% The directoty path is automatically found
%   
    %cxxFlags = ['-std=c++11 -fPIC -fno-omit-frame-pointer -pthread -fexceptions'];
    cxxFlags = ['-std=c++11'];
    
    iPath = {['-I' fullfile('../')], ['-I' fullfile('../libs/include/')]};
    lPath = {['-L' fullfile('../libs/')]};

    srcPath = '../';
    libsPath = '../libs/';
    trgPath = '';
    
    objDst = 'build';
    trgDst = '+bin';
    
    
    src = {...
        'Code.cpp'; ...
        'CodeStructure/CodeStructure.cpp'; ... 
        'CodeStructure/ConvolutionalCodeStructure.cpp'; ...
        'CodeStructure/TurboCodeStructure.cpp'; ...
        'CodeStructure/LdpcCodeStructure.cpp'; ...
        'CodeStructure/TrellisStructure.cpp'; ...
        'ConvolutionalCode/ConvolutionalCode.cpp'; ...
        'ConvolutionalCode/MapDecoder/MapDecoder.cpp'; ...
        'ConvolutionalCode/ViterbiDecoder/ViterbiDecoder.cpp'; ...
        'TurboCode/TurboCode.cpp'; ...
        'TurboCode/TurboCodeImpl.cpp'; ...
        'LdpcCode/LdpcCode.cpp'; ...
        'LdpcCode/BpDecoder/BpDecoder.cpp'; ...
    };

    libs = {
        'serialization/src/archive_exception.cpp'; ...
        'serialization/src/basic_archive.cpp'; ...
        'serialization/src/basic_iarchive.cpp'; ...
        'serialization/src/basic_iserializer.cpp'; ...
        'serialization/src/basic_oarchive.cpp'; ...
        'serialization/src/basic_oserializer.cpp'; ...
        'serialization/src/basic_pointer_iserializer.cpp'; ...
        'serialization/src/basic_pointer_oserializer.cpp'; ...
        'serialization/src/basic_serializer_map.cpp'; ...
        'serialization/src/basic_text_iprimitive.cpp'; ...
        'serialization/src/basic_text_oprimitive.cpp'; ...
        'serialization/src/basic_xml_archive.cpp'; ...
        'serialization/src/binary_iarchive.cpp'; ...
        'serialization/src/binary_oarchive.cpp'; ...
        'serialization/src/codecvt_null.cpp'; ...
        'serialization/src/extended_type_info_no_rtti.cpp'; ...
        'serialization/src/extended_type_info_typeid.cpp'; ...
        'serialization/src/extended_type_info.cpp'; ...
        'serialization/src/polymorphic_iarchive.cpp'; ...
        'serialization/src/polymorphic_oarchive.cpp'; ...
        'serialization/src/stl_port.cpp'; ...
        'serialization/src/text_iarchive.cpp'; ...
        'serialization/src/text_oarchive.cpp'; ...
        'serialization/src/utf8_codecvt_facet.cpp'; ...
        'serialization/src/void_cast.cpp';...
        'serialization/src/xml_archive_exception.cpp'; ...
        'serialization/src/xml_iarchive.cpp'; ...
        'serialization/src/xml_oarchive.cpp'; ...
        'serialization/src/xml_grammar.cpp'; ...
        };
    
    trg = {...
        '@TurboCode/TurboCode_constructor.cpp'; ...
        '@Code/Code_get_paritySize.cpp'; ...
        '@Code/Code_get_msgSize.cpp'; ...
        '@Code/Code_get_extrinsicSize.cpp'; ...
        '@Code/Code_encode.cpp'; ...
        '@Code/Code_decode.cpp'; ...
        '@Code/Code_softOutDecode.cpp'; ...
        '@Code/Code_appDecode.cpp'; ...
        '@Code/Code_destructor.cpp'; ...
        '@Code/Code_save.cpp'; ...
        '@Code/Code_load.cpp'; ...
        '@ConvolutionalCode/ConvolutionalCode_constructor.cpp'; ...
        '@LdpcCode/LdpcCode_constructor.cpp'
        };
    
    
    
    oldpath = cd(strrep(which(fullfile('+fec', 'make.m')), fullfile('make.m'), ''));
    mkdir(objDst);
    mkdir(trgDst);
    
    objs = '';
    for i = 1:length(src)
        [pathstr,name,ext] = fileparts(src{i});
        objInfo = dir(['build/' name '.*']);
        srcInfo = dir([srcPath src{i}]);
        if (isempty(objInfo) || objInfo.datenum < srcInfo.datenum)
            mex(['CXXFLAGS=$CXXFLAGS ' cxxFlags], iPath{:}, '-largeArrayDims', '-outdir', objDst, '-c', [srcPath src{i}]);
        else
            disp('skip');
        end
        objInfo = dir(['build/' name '.*']);
        obj = fullfile('build', objInfo.name);
        objs{length(objs)+1} = obj;
    end
    for i = 1:length(libs)
        [pathstr,name,ext] = fileparts(libs{i});
        objInfo = dir(['build/' name '.*']);
        srcInfo = dir([libsPath libs{i}]);
        if (isempty(objInfo) || objInfo.datenum < srcInfo.datenum)
            mex(['CXXFLAGS=$CXXFLAGS ' cxxFlags], iPath{:}, '-largeArrayDims', '-outdir', objDst, '-c', [libsPath libs{i}]);
        else
            disp('skip');
        end
        objInfo = dir(['build/' name '.*']);
        obj = fullfile('build', objInfo.name);
        objs{length(objs)+1} = obj;
    end

    for i = 1:length(trg)
        mex(['CXXFLAGS=$CXXFLAGS ' cxxFlags], iPath{:},lPath{:}, '-largeArrayDims', '-outdir', trgDst, [trgPath trg{i}], objs{:});
    end
    
    cd(oldpath)
end
    