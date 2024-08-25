#include "data_struct.cpp"

//Index class defination

void Index::select_component(bool x){
			// 1 in x indicate folder search and 0 indicate file search
			std::filesystem::path p = std::filesystem::current_path();
			std::filesystem::path temp;
			string query_t = (x)?"Folder":"File";
			cout<<query_t<<endl;
			cout<<"\nOperation to search " <<query_t<<endl;
			int index, choice;
			bool result;
			do {
				result = false;
				cout<<"\nSelect the "<<query_t<<" to operate.\n";
				cout<<"\n1.Go back\n";
				index = 1;

				for (auto itr : std::filesystem::directory_iterator(p)){
						index++ ;
						cout<<index<<". "<<itr<<endl;
				}
				cin>>choice;

				if (choice == 1){
					p = p.parent_path();
					continue;
				}
				else if( choice > 1){
					index = 2;
					for (auto itr: std::filesystem::directory_iterator(p)){
						if (choice == index){
							temp = itr;
							break;
						}
						index ++;
					}
				}

				cout<<temp<<endl;
				//validation based in input ie. folder, file verif
				if (x){
					result = is_directory(temp);
					if (result){
						cout<<"\n1. Select the folder\n2. Move inside \n-->> "<<endl;
						cin>>choice;
						if (choice ==1 ){
							//read_indexfile(temp);
							defragmentation(temp);
						}
						else if (choice == 2){
							result = false;
							p = temp;
						}
						else{ 
							cout<<"Error";
							result=false;
						}
					}
				}
				else {
					
					result = is_directory(temp);
					result = !result; 		// it inverts the result 
					p = temp;		//to avoid the current directory deadlock
					if (result){
						extract_origInfo(p);
						temp = generate_indexfile(p);
						fragmentation(p);
					}
				}

			}while(result == false);
			p = temp;
			cout<<"\nThe correct location is : "<<p<<endl;
}

std::string Index::computeHash(const std::vector<char>& data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data.data()), data.size(), hash);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    return ss.str();
}


void Index::extract_origInfo(std::filesystem::path pt){
	file_name = pt.filename().string();
	file_size = std::filesystem::file_size(pt);	
	cout<<"\nEnter the size of block , in bytes : ";
	cin>>block_size;
	no_blocks = ceil((float)file_size / (float)block_size);
	stat(file_name.c_str(), &file_stat);
	
	cout<<"\n"<<pt;

	std::ifstream fileStream(pt, std::ios::binary);
		if(!fileStream.is_open()){
			std::cerr<< "Error: Cannot open file " <<pt << "for reading."<<endl;
			return;
		}
		std::vector<char> fileData(file_size);
		fileStream.read(fileData.data(),file_size);
		fileStream.close();

		fileHash = computeHash(fileData);

	cout<<"\nThe name of file is : "<< file_name; 	
	cout<<"\nThe size of the file is : "<<file_size;	
	cout<<"\nThe block size is : "<< block_size;
	cout<<"\nTotal no of block's division is : "<<no_blocks;
	cout<<"\nThe last modification , access and creation date of file is : "<< file_stat.st_mtime <<" , ";
	cout<<file_stat.st_atime<<" , "<<file_stat.st_ctime<<endl;	
	cout<<"\nThe generated hash of file is "<<fileHash<<endl;
}

std::filesystem::path Index::generate_indexfile(std::filesystem::path pt){
	std::filesystem::path temp = pt.parent_path(); 
	temp /= pt.stem();
	std::filesystem::create_directory(temp);
	int len, Hlen;
	if (std::filesystem::exists(temp)){
		std::filesystem::path file_path = temp;
		file_path /= "index.dat";
	
		std::fstream indexStream(file_path, std::ios::out);
		if (indexStream.good()){
			cout<<"\nindex.dat file opened sucessfully.";
			len = file_name.length();
	
			indexStream.write(reinterpret_cast<char *> (&len), sizeof(len));
			indexStream.write(file_name.c_str(), len);
			indexStream.write(reinterpret_cast<char *> (&file_size) , sizeof(unsigned int));
			indexStream.write(reinterpret_cast<char *> (&block_size), sizeof(unsigned int));
			indexStream.write(reinterpret_cast<char *> (&no_blocks) , sizeof(unsigned int));
			indexStream.write(reinterpret_cast<char *> (&file_stat), sizeof(struct stat));
			
			Hlen = fileHash.length();
			indexStream.write(reinterpret_cast<char *> (&Hlen), sizeof(int));
			indexStream.write(fileHash.c_str(), Hlen);
			
			indexStream.close();			
			return temp;
		}
		else {
			cout<<"\nCannot open the file for the operation";
		}
	}
	else {
		cout<<"\nCannot create the directory : "<<pt.stem()<<" over the location. \n";
		cout<<temp;
	}
	return pt;
}

void Index::read_indexfile(std::filesystem::path pt){
	std::ifstream indexStream;
	int len,fs, bs, ns, Hlen;
	struct stat temp_fs;
	char *buffer, *Hbuffer;

	if (std::filesystem::exists(pt)){
		pt /= "index.dat";
		indexStream.open(pt,std::ios::in);
		if (! indexStream.good()){
			cout<<"Error occured. "<<file_name<<" not found."<<endl;
		}
		else{
			cout<<"index.dat opened for reading sucessfully"<<endl;
			indexStream.read(reinterpret_cast<char *> (&len), sizeof(int));
			buffer = new char[len +1];
			indexStream.read(buffer, len);
			buffer[len] = '\0';
			
			indexStream.read(reinterpret_cast<char *> (&fs), sizeof(int));
			indexStream.read(reinterpret_cast<char *> (&bs), sizeof(int));
			indexStream.read(reinterpret_cast<char *> (&ns), sizeof(int));
			indexStream.read(reinterpret_cast<char *> (&temp_fs) , sizeof(struct stat));
			
			indexStream.read(reinterpret_cast<char *> (&Hlen), sizeof(int));
			Hbuffer = new char[Hlen +1];
			indexStream.read(Hbuffer, Hlen);
			Hbuffer[Hlen] = '\0';

			indexStream.close();
			
			file_name = string(buffer);
			file_size = fs;
			block_size = bs;
			no_blocks = ns;
			file_stat = temp_fs;
			fileHash = string(Hbuffer);

			delete [] buffer;
			delete [] Hbuffer;		
	
			cout<<"\nData recieved are : "<<endl;

			cout<<"File name : "<<file_name<<endl;
			cout<<"File size : "<<file_size<<endl;
			cout<<"Block size : "<<block_size<<endl;
			cout<<"No of blocks : "<<no_blocks<<endl;
			cout<<"\nCreattion date : "<<file_stat.st_ctime<<endl;
			cout<<"\nModification date : "<<file_stat.st_mtime<<endl;
			cout<<"\nThe generated hash of file is "<<fileHash<<endl;
	
		}
	}	
}

void Index::fragmentation(std::filesystem::path pt) {
			std::vector<std::string> fragment(no_blocks);
			std::vector<char> buffer(block_size, 0);
			std::fstream fin(pt, std::ios::in | std::ios::binary);
			if (!fin.is_open()) {
				cout << "Unable to open file";
				cout << file_name;
			} else {
				int i = 0;
				while (!fin.eof()) {
					fin.read(buffer.data(), block_size);
					std::streamsize bytes_read = fin.gcount();
					fragment[i] = std::string(buffer.data(), bytes_read);
					i++;
					if (i >= no_blocks) break; 
				}
				int a ;
				// std::filesystem::path index_path = pt.parent_path() / pt.stem();
				std::filesystem::path index_path = pt;
				// cout<<index_path;
				// cin>>a;
				Fragment frag(no_blocks, block_size);
				frag.packaging(&fragment, index_path);										}
}

Fragment::Fragment(unsigned int a, unsigned int b){
	total_blocks = a;
	fragment_size = b;
}

void Fragment::packaging(std::vector<std::string> *fragment_array, std::filesystem::path fragment_location){
	std::filesystem::path parentDirectory = fragment_location.parent_path();
	parentDirectory /= fragment_location.stem();

	std::string fileNameWoExtension = fragment_location.stem().string();

	if(fragment_array == nullptr){
		std::cerr<<"Error: Null Address Supplied to the function"<<endl;
	}
	// It means assign value of fragment[first] to fragment[last] assign value to frag
	int k = 0;
	for (const auto &frag: *fragment_array){
		std::string temp =  fileNameWoExtension + std::to_string(k)+ ".dat";
		std::filesystem::path fileName = parentDirectory ;
		fileName /= temp;

		cout<<"\nThe File location is: "<<fileName<<endl;
		if(std::filesystem::exists(parentDirectory)){		
			std::ofstream fragFileStream(fileName, std::ios::binary);
			if (!fragFileStream.good()){
				std::cerr<<"Error: Error encountered while writing to fragment with name: "<<fileName<<endl;
				std::cerr<<"Error: "<<strerror(errno)<<endl;
				return;
			}
			else{
				fragment_no = k;
				prev_frag_no = k-1;
				if(k < total_blocks-1){
					next_frag_no = k+1;
					}
				else if(k == total_blocks-1){
						next_frag_no = 0;
						}
				serialization(fragFileStream, frag);
				k++;
				}
		}else{
			cout<<"Parent Directory: '"<<parentDirectory<<"' does not exist";
		}
	}
}

void Fragment::serialization(std::ofstream &out, std::string frag_data){

	// Serialize fragment data 
	size_t fragLength = frag_data.size();
	out.write(reinterpret_cast<char *>(&fragLength), sizeof(fragLength));
	out.write(frag_data.c_str(), fragLength);

	// Serialize unsigned integer
	out.write(reinterpret_cast<char *>(&fragment_no), sizeof(fragment_no));
	out.write(reinterpret_cast<char *>(&fragment_size), sizeof(fragment_size));
	out.write(reinterpret_cast<char *>(&prev_frag_no), sizeof(prev_frag_no));
	out.write(reinterpret_cast<char *>(&next_frag_no), sizeof(next_frag_no));

	// Serialize String
	size_t fragNoLength = frag_no.size();
	out.write(reinterpret_cast<char *>(&fragNoLength), sizeof(fragNoLength));
	out.write(frag_no.c_str(), fragNoLength);

	// size_t dataLength = data.size();
	// out.write(reinterpret_cast<char *>(&dataLength), sizeof(dataLength));
	// out.write(data.c_str(), dataLength); 
}

void Fragment::deserialization(std::ifstream &in) {

	size_t fragLength;
    in.read(reinterpret_cast<char*>(&fragLength), sizeof(fragLength));
	
	if (fragLength > 0) {
        data.resize(fragLength);
        in.read(&data[0], fragLength);
    } else {
        std::cerr << "Warning: Fragment data length is zero." << std::endl;
    }


	in.read(reinterpret_cast<char*>(&fragment_no), sizeof(fragment_no));
	in.read(reinterpret_cast<char*>(&fragment_size), sizeof(fragment_size));
	in.read(reinterpret_cast<char*>(&prev_frag_no), sizeof(prev_frag_no));
	in.read(reinterpret_cast<char*>(&next_frag_no), sizeof(next_frag_no));

	size_t fragNoLength;
	in.read(reinterpret_cast<char*>(&fragNoLength), sizeof(fragNoLength));
	frag_no.resize(fragNoLength);
    in.read(&frag_no[0], fragNoLength);
	}

void Index::defragmentation(std::filesystem::path pt){
	std::filesystem::path x=pt;
	struct utimbuf new_fs;

	if(!std::filesystem::exists(pt) || !std::filesystem::is_directory(pt) || !std::filesystem::exists(x /"index.dat"))
		{
		std::cerr << "Error: Path does not exist or is not a directory." << endl;
		return;
		}

	// Create temporary path
		std::filesystem::path temp = pt;

		read_indexfile(pt);

		std::filesystem::path file_path(file_name);
		std::filesystem::path outputfile = pt.parent_path();
		outputfile /= (pt.stem().string() + "_original" + file_path.extension().string());
		file_name = outputfile.filename().string();//
	
		std::fstream defrag;
		defrag.open(outputfile, std::ios::out | std::ios::binary );
			if(!defrag.is_open()){
				std::cerr << "Error writing to " << outputfile << endl;
				return;
			}
			//defrag.write(reinterpret_cast<char *> (&file_size) , sizeof(unsigned int));
			// new_fs.actime = file_stat.st_atime;
			// new_fs.modtime = file_stat.st_mtime;
			//     if (utime(file_name.c_str(), &new_fs) != 0) {
        	// 		std::cerr << "Failed to set file times for " << outputfile << std::endl;
   			// 	}
	
    // for (unsigned int i = 0; i < no_blocks; ++i) {
    //     std::filesystem::path fragment_path = pt / (pt.stem().string() + std::to_string(i) + ".dat");
		std::vector<Fragment> fragments;
 		for (const auto& fragment_path : std::filesystem::directory_iterator(pt)) {
    	if (fragment_path.is_regular_file() && fragment_path.path().extension() == ".dat" && fragment_path.path().filename() != "index.dat") {
        	std::ifstream fragmentStream(fragment_path.path(), std::ios::in | std::ios::binary);
        	if (!fragmentStream.is_open()) {
            std::cerr << "Unable to open fragment file: " << fragment_path << std::endl;
            return;
        }
        	Fragment meta;
       		meta.deserialization(fragmentStream);
        	fragments.push_back(meta);
   			}
		}

	 std::sort(fragments.begin(), fragments.end(), [](const Fragment& a, const Fragment& b) {
		return a.get_fragment_no() < b.get_fragment_no();
    });

    // Write fragments to the defragmented file in order
    for (const auto& fragment : fragments) {
        defrag.write(fragment.get_data().data(), fragment.get_data().size());
	}
	defrag.close();

	defrag.open(outputfile,std::ios::in | std::ios::binary);
	std::vector<char> fileData(file_size);
		defrag.read(fileData.data(),file_size);
		defrag.close();
		std::string NewHash = computeHash(fileData);

		if(NewHash.compare(fileHash) == 0)
			std::cout<<"Hash are equal"<<endl;
		else{
			std::cerr<<"Hash not equal"<<endl;
			std::cerr<<"File is corrupt"<<endl;
			return;
		}

    std::cout << "Defragment successful" << std::endl;
}
 		